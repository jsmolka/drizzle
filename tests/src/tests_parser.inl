using AstNodeTypes = std::vector<std::variant<Statement::Type, Expression::Type>>;

class AstNodeComparer final : public AstWalker
{
public:
    void compare(Stmt& ast, const AstNodeTypes& expected)
    {
        current = expected.begin();
        end = expected.end();

        walk(ast);
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

void parse(const std::string& source, const AstNodeTypes& expected)
{
    auto tokens = Tokenizer().tokenize(source);
    auto ast = Parser().parse(tokens);
    AstNodeComparer().compare(ast, expected);
}
