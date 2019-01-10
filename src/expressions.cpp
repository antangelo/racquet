//
// Created by Antonio on 2019-01-09.
//

#include "parser.h"

namespace Expressions
{
    std::ostream &operator<<(std::ostream &stream, const Expression &expr)
    {
        stream << expr.toString();
        return stream;
    }

    /* PartialExpression */

    inline bool PartialExpression::isValue()
    {
        return false;
    }

    std::unique_ptr<Expression> PartialExpression::evaluate(std::unique_ptr<Expression> *obj_ref)
    {
        Expressions::expression_vector members;

        for (const auto &str : mTupleMembers)
        {
            std::unique_ptr<Expressions::Expression> expr;
            auto parseSuccessful = Parser::parse(str, expr);

            if (!parseSuccessful)
                throw std::invalid_argument(
                        "Parsing failed"); //TODO: Write exception for syntax errors/unsuccessful parsing.

            if (auto partialExpression = dynamic_cast<PartialExpression *>(expr.get()))
            {
                // Not the best, but obj_ref isn't used by evaluate() so it ends up saving
                // unnecessary pointer creation with a PartialExpression here.
                members.push_back(std::move(partialExpression->evaluate(obj_ref)));
            } else
            {
                members.push_back(std::move(expr));
            }
        }

        std::unique_ptr<TupleExpression> expr(new TupleExpression(std::move(members)));
        return std::move(expr);
    }

    std::string PartialExpression::toString() const
    {
        std::string str = "(";

        for (const auto &i : mTupleMembers)
        {
            str += i + " ";
        }

        return str.substr(0, str.size() - 1) + ")";
    }

    /* TupleExpression */

    bool TupleExpression::isValue()
    {
        return false; // for now, will change in the future.
    }

    std::unique_ptr<Expression> TupleExpression::evaluate(std::unique_ptr<Expression> *obj_ref)
    {
        for (auto &expr : mTupleMembers)
        {
            if (!expr->isValue())
            {
                expr = expr->evaluate(&expr);
                return std::move(*obj_ref);
            }
        }

        if (auto func = dynamic_cast<FunctionExpression *>(mTupleMembers.front().get()))
        {
            mTupleMembers.erase(mTupleMembers.begin());
            return func->call(std::move(mTupleMembers));
        }

        return std::move(*obj_ref);
    }

    std::string TupleExpression::toString() const
    {
        std::string str = "(";

        for (const auto &i : mTupleMembers)
        {
            str += i->toString() + " ";
        }

        return str.substr(0, str.size() - 1) + ")";
    }

    /* FunctionExpression */
    inline bool FunctionExpression::isValue()
    {
        return true;
    }

    inline std::unique_ptr<Expression> FunctionExpression::evaluate(std::unique_ptr<Expressions::Expression> *obj_ref)
    {
        return std::move(*obj_ref);
    }

    std::string FunctionExpression::toString() const
    {
        return mFuncName;
    }

    std::unique_ptr<Expression> FunctionExpression::call(Expressions::expression_vector args)
    {
        return mFunction(std::move(args));
    }

    /* NumericalValueExpression */

    bool NumericalValueExpression::isValue()
    {
        return true;
    }

    std::unique_ptr<Expression> NumericalValueExpression::evaluate(std::unique_ptr<Expression> *obj_ref)
    {
        return std::move(*obj_ref);
    }

    std::string NumericalValueExpression::toString() const
    {
        return std::to_string(mValue);
    }
}
