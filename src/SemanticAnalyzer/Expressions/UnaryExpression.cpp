#include "Ast/Expressions.hpp"
#include "Utils/Macros.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"

namespace Marble
{
    Ref<TypeSpecifier> UnaryExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_UnaryExpressionType)
        {
        case UnaryExpressionType::PostFix:
            return AnalyzePostFix(semanticAnalyzer);
        case UnaryExpressionType::Prefix:
            return AnalyzePrefix(semanticAnalyzer);
        }
        UNREACHABLE();
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzePostFix(SemanticAnalyzer &semanticAnalyzer)
    {

        switch (m_Value->ExpressionType())
        {
        case ExpressionType::Identifier:
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        case ExpressionType::Unary:
            break;
        default:
            throw "Unexpected expression";
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze(semanticAnalyzer);
        CheckType(expressionType);
        return expressionType;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzePrefix(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_UnaryOperator)
        {
        case UnaryOperators::Address:
            return AnalyzeAddress(semanticAnalyzer);
        case UnaryOperators::Indirection:
            return AnalyzePointer(semanticAnalyzer);
        case UnaryOperators::Plus:
        case UnaryOperators::Minus:
            return AnalyzeArithmetic(semanticAnalyzer);
        case UnaryOperators::Increment:
        case UnaryOperators::Decrement:
            return AnalyzePostFix(semanticAnalyzer);
        case UnaryOperators::LogicalNot:
        case UnaryOperators::BitwiseNot:
            return AnalyzeNot(semanticAnalyzer);
        }
        UNREACHABLE();
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzeArithmetic(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_Value->ExpressionType())
        {
        case ExpressionType::ObjectInit:
        case ExpressionType::ArrayInit:
            throw "Unexpected expression";
        default:
            break;
        }
        Ref<TypeSpecifier> expressionType = m_Value->Analyze(semanticAnalyzer);
        CheckType(expressionType);
        return expressionType;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzeAddress(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_Value->ExpressionType())
        {
        case ExpressionType::Identifier:
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        case ExpressionType::Unary:
            break;
        default:
            throw "Unexpected expression";
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze(semanticAnalyzer);
        return MakeRef<TypeSpecifier>(PointerType{expressionType}, Span{});
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzePointer(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_Value->ExpressionType())
        {
        case ExpressionType::Identifier:
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        case ExpressionType::Unary:
            break;
        default:
            throw "Unexpected expression";
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze(semanticAnalyzer);
        Types type = expressionType->GetType();
        if (type != Types::Pointer)
        {
            throw "Expected pointer type";
        }
        auto &pointerType = expressionType->Pointer();
        return pointerType.TypeSpecifier;
    }

    Ref<TypeSpecifier> UnaryExpression::AnalyzeNot(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_Value->ExpressionType())
        {
        case ExpressionType::ObjectInit:
        case ExpressionType::ArrayInit:
            throw "Unexpected expression";
        default:
            break;
        }

        Ref<TypeSpecifier> expressionType = m_Value->Analyze(semanticAnalyzer);
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
        UNREACHABLE();
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

    void UnaryExpression::SubstituteGenerics(const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Value->SubstituteGenerics(map);
    }

    Box<Expression> UnaryExpression::Clone()
    {
        return MakeBox<UnaryExpression>(*this);
    }
} // namespace Marble
