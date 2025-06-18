#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
namespace Marble
{
    Ref<TypeSpecifier> ArrayIndexExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        AnalyzeIndex(semanticAnalyzer, m_Index.get());
        if (m_SecondIndex)
        {
            AnalyzeIndex(semanticAnalyzer, m_SecondIndex.get());
        }
        Marble::ExpressionType arrayType = m_Array->ExpressionType();
        if (arrayType != ExpressionType::Identifier && arrayType != ExpressionType::MemberAccess && arrayType != ExpressionType::FunctionCall)
        {
            throw "Array type must be Identifier, Member Access or Function Call expression";
        }
        Ref<TypeSpecifier> exprType = m_Array->Analyze(semanticAnalyzer);
        if (exprType->GetType() == Types::ArrayType)
        {
            const ArrayType &arrayType = exprType->Array();
            if (arrayType.TypeSpecifier->GetType() == Types::Str)
            {
                return MakeRef<TypeSpecifier>(Types::Char);
            }
            if (arrayType.TypeSpecifier->GetType() == Types::ArrayType)
            {
                const ArrayType &arrayTypeSecond = arrayType.TypeSpecifier->Array();
                return arrayTypeSecond.TypeSpecifier;
            }
            return arrayType.TypeSpecifier;
        }

        if (exprType->GetType() == Types::Pointer)
        {
            return exprType->Pointer().TypeSpecifier;
        }
        throw "Expect the array type";
    }

    void ArrayIndexExpression::AnalyzeIndex(SemanticAnalyzer &semanticAnalyzer, Expression *indexExpression)
    {
        Marble::ExpressionType expressionType = indexExpression->ExpressionType();
        if (expressionType == ExpressionType::ArrayInit || expressionType == ExpressionType::ObjectInit)
        {
            throw "Index cannot be an object or array init expression";
        }

        Ref<TypeSpecifier> indexExprType = indexExpression->Analyze(semanticAnalyzer);
        if (indexExprType->GetType() == Types::Usize)
        {
            return;
        }
        throw "Array index must be usize";
    }

    Box<Expression> ArrayIndexExpression::Clone()
    {
        return MakeBox<ArrayIndexExpression>(*this);
    }
} // namespace Marble