using AstNodeTypes = std::vector<std::variant<Statement::Type, Expression::Type>>;

class AstNodeComparer final : public AstWalker
{
public:
    AstNodeComparer(const AstNodeTypes& compare)
        : compare(compare) {}

    void run(Stmt& ast)
    {
        iter = compare.begin();
        walk(ast);
        REQUIRE(iter == compare.end());
    }

protected:
    void before(Stmt& stmt) final
    {
        REQUIRE(iter != compare.end());
        REQUIRE(iter->index() == 0);
        REQUIRE(stmt->type == std::get<0>(*iter));
        iter++;
    }

    void before(Expr& expr) final
    {
        REQUIRE(iter != compare.end());
        REQUIRE(iter->index() == 1);
        REQUIRE(expr->type == std::get<1>(*iter));
        iter++;
    }

private:
    const AstNodeTypes& compare;
    AstNodeTypes::const_iterator iter;
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

void parse(const std::string& source, const AstNodeTypes& compare)
{
    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);
    AstNodeComparer(compare).run(ast);
}

template<typename T>
void parseTest(const std::string& source, const typename AstNodeTester<T>::Test& test)
{
    const auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);
    AstNodeTester(test).run(ast);
}
