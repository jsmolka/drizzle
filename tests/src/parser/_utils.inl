using AstNodeTypes = std::vector<std::variant<Statement::Type, Expression::Type>>;

class AstNodeComparer final : public AstWalker
{
public:
    void compare(Stmt& ast, const AstNodeTypes& expected)
    {
        current = expected.begin();
        end = expected.end();

        walk(ast);

        REQUIRE(current == end);
    }

protected:
    void before(Stmt& stmt) final
    {
        REQUIRE(current != end);
        REQUIRE(current->index() == 0);
        REQUIRE(stmt->type == std::get<0>(*current));
        current++;
    }

    void before(Expr& expr) final
    {
        REQUIRE(current != end);
        REQUIRE(current->index() == 1);
        REQUIRE(expr->type == std::get<1>(*current));
        current++;
    }

private:
    AstNodeTypes::const_iterator current;
    AstNodeTypes::const_iterator end;
};

template<typename T>
class AstNodeTester final : public AstWalker
{
public:
    static_assert(std::is_same_v<T, Expr> || std::is_same_v<T, Stmt>);

    using Test = std::function<void(T&)>;

    AstNodeTester(const Test& test)
        : test(test) {}

    void run(Stmt& ast)
    {
        walk(ast);
    }

protected:
    void before(Stmt& stmt) final
    {
        if constexpr (std::is_same_v<T, Stmt>)
            test(stmt);
    }

    void before(Expr& expr) final
    {
        if constexpr (std::is_same_v<T, Expr>)
            test(expr);
    }

private:
    const Test& test;
};

void parse(const std::string& source, const AstNodeTypes& expected)
{
    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);
    AstNodeComparer().compare(ast, expected);
}

template<typename T>
void parseTest(const std::string& source, const typename AstNodeTester<T>::Test& test)
{
    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);
    AstNodeTester(test).run(ast);
}
