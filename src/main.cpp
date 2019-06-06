#include <iostream>

#include "interpret/interpret.h"
#include "functions/functions.h"

void repl(std::shared_ptr<Expressions::Scope> &globalScope)
{
    bool hideSteps = true;

    for (;;)
    {
        try
        {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "(exit)") break;
            if (input == "(toggle-step)") hideSteps = !hideSteps;
            else
            {
                auto expr = Parser::parse(input, globalScope);

                if (hideSteps)
                {
                    expr = Interpreter::interpret(std::move(expr));
                    if (dynamic_cast<Expressions::VoidValueExpression *>(expr.get())) continue;

                    std::cout << expr->toString() << std::endl;
                }
                else
                {
                    Expressions::expression_vector steps = Interpreter::interpretSaveSteps(std::move(expr));

                    for (auto &exp : steps)
                    {
                        if (dynamic_cast<Expressions::VoidValueExpression *>(exp.get()) ||
                            dynamic_cast<Expressions::PartialExpression *>(exp.get()))
                            continue;

                        std::cout << exp->toString() << std::endl;
                    }
                }
            }
        }
        catch (std::exception &exception)
        {
            std::cout << exception.what() << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Racket Interpreter Alpha 1" << std::endl;
    std::cout << "Run '(exit)' to exit." << std::endl;

    Functions::registerFunctions();

    std::shared_ptr<Expressions::Scope> globalScope(new Expressions::Scope(nullptr));

    repl(globalScope);

    globalScope->clear();

    return 0;
}