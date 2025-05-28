#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "Utils/Macros.hpp"

namespace Marble
{
    Ref<TypeSpecifier> UnaryExpression::Analyze()
    {
        switch (m_UnaryExpressionType)
        {
        case UnaryExpressionType::PostFix:
            return AnalyzePostFix();
        case UnaryExpressionType::Prefix:
            return AnalyzePrefix();
        }
        UNREACHABLE;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzePostFix()
    {

        switch (m_ExpressionType)
        {
        case ExpressionType::Identifier:
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        case ExpressionType::Unary:
            break;
        default:
            throw "Unexpected expression";
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze();
        CheckType(expressionType);
        return expressionType;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzePrefix()
    {
        switch (m_UnaryOperator)
        {
        case UnaryOperators::Address:
            return AnalyzeAddress();
        case UnaryOperators::Indirection:
            return AnalyzePointer();
        case UnaryOperators::Plus:
        case UnaryOperators::Minus:
            return AnalyzeArithmetic();
        case UnaryOperators::Increment:
        case UnaryOperators::Decrement:
            return AnalyzePostFix();
        case UnaryOperators::LogicalNot:
        case UnaryOperators::BitwiseNot:
            return AnalyzeNot();
        }
        UNREACHABLE;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzeArithmetic()
    {
        switch (m_ExpressionType)
        {
        case ExpressionType::ObjectInit:
        case ExpressionType::ArrayInit:
            throw "Unexpected expression";
        default:
            break;
        }
        Ref<TypeSpecifier> expressionType = m_Value->Analyze();
        CheckType(expressionType);
        return expressionType;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzeAddress()
    {
        switch (m_ExpressionType)
        {
        case ExpressionType::Identifier:
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        case ExpressionType::Unary:
            break;
        default:
            throw "Unexpected expression";
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze();
        return MakeRef<TypeSpecifier>(PointerType{expressionType}, Span{});
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzePointer()
    {
        switch (m_ExpressionType)
        {
        case ExpressionType::Identifier:
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        case ExpressionType::Unary:
            break;
        default:
            throw "Unexpected expression";
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze();
        Types type = expressionType->GetType();
        if (type != Types::Pointer)
        {
            throw "Expected pointer type";
        }
        auto &pointerType = expressionType->Pointer();
        return pointerType.TypeSpecifier;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzeNot()
    {
        switch (m_ExpressionType)
        {
        case ExpressionType::ObjectInit:
        case ExpressionType::ArrayInit:
            throw "Unexpected expression";
        default:
            break;
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze();
        Types type = expressionType->GetType();

        if (m_UnaryOperator == UnaryOperators::BitwiseNot)
        {
            if (type == Types::Int || type == Types::Usize)
            {
                return expressionType;
            }
            throw "Type not supported for the operation";
        }

        if (m_UnaryOperator == UnaryOperators::LogicalNot)
        {
            if (type == Types::Bool)
            {
                return expressionType;
            }
            throw "Type not supported for the operation";
        }
        UNREACHABLE;
    }

    void UnaryExpression::CheckType(Ref<TypeSpecifier> expressionType)
    {
        Types type = expressionType->GetType();

        switch (type)
        {
        case Types::Int:
        case Types::Float:
        case Types::Usize:
        case Types::Double:
            return;
        default:
            throw "Value is not supported for the operator";
        }
    }
} // namespace Marble
