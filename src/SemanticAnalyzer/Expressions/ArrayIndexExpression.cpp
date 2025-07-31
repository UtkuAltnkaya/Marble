#include "Ast/Expressions.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/CompilerError.hpp"

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
            ErrorSystem::AddError(semanticAnalyzer, this, "Array type must be Identifier, Member Access or Function Call expression");
        }

        Ref<TypeSpecifier> exprType = m_Array->Analyze(semanticAnalyzer);

        if (exprType->GetType() == Types::ArrayType)
        {
            const ArrayType &arrayType = exprType->Array();
            if (arrayType.TypeSpecifier->GetType() == Types::Str)
            {
                m_ValueType = MakeRef<TypeSpecifier>(Types::Char);
                return m_ValueType;
            }
            if (arrayType.TypeSpecifier->GetType() == Types::ArrayType)
            {
                const ArrayType &arrayTypeSecond = arrayType.TypeSpecifier->Array();
                m_ValueType = arrayTypeSecond.TypeSpecifier;
                return m_ValueType;
            }
            m_ValueType = arrayType.TypeSpecifier;
            return m_ValueType;
        }
        if (exprType->GetType() == Types::Pointer)
        {
            m_ValueType = exprType->Pointer().TypeSpecifier;
            return m_ValueType;
        }
        if (exprType->GetType() == Types::Str)
        {
            m_ValueType = MakeRef<TypeSpecifier>(Types::Char);
            return m_ValueType;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Expect the array type");
        return TypeSpecifierVoid;
    }

    void ArrayIndexExpression::AnalyzeIndex(SemanticAnalyzer &semanticAnalyzer, Expression *indexExpression)
    {
        Marble::ExpressionType expressionType = indexExpression->ExpressionType();
        if (expressionType == ExpressionType::ArrayInit || expressionType == ExpressionType::ObjectInit)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Index cannot be an object or array init expression");
            return;
        }

        Ref<TypeSpecifier> indexExprType = indexExpression->Analyze(semanticAnalyzer);
        if (indexExprType->GetType() == Types::Usize)
        {
            return;
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Array index must be usize");
    }

    void ArrayIndexExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Array->SubstituteGenerics(semanticAnalyzer, map);
        m_Index->SubstituteGenerics(semanticAnalyzer, map);
        if (m_SecondIndex)
        {
            m_SecondIndex->SubstituteGenerics(semanticAnalyzer, map);
        }
    }

    Box<Expression> ArrayIndexExpression::Clone()
    {
        return MakeBox<ArrayIndexExpression>(*this);
    }
} // namespace Marble