#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    Ref<TypeSpecifier> MemberAccessExpression::Analyze()
    {
        CheckObjectExpressionType();
        Ref<TypeSpecifier> objectType = m_Object->Analyze();

        const Identifier *identifier;
        if (objectType->GetType() == Types::UserDefine)
        {
            if (m_AccessType != TokenType::Dot)
            {
                throw "Use dot('.') operator to access member";
            }
            identifier = &objectType->UserDefine();
        }
        else if (objectType->GetType() == Types::Pointer)
        {
            const PointerType &pointer = objectType->Pointer();
            Ref<TypeSpecifier> pointerType = pointer.TypeSpecifier;
            if (pointerType->GetType() != Types::UserDefine)
            {
                throw "Member access only can use with user define type";
            }
            if (m_AccessType != TokenType::Arrow)
            {
                throw "Use arrow('->') operator to access member with pointer type";
            }
            identifier = &pointerType->UserDefine();
        }
        else
        {
            throw "Member access only can use with user define type";
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *iter = table.Iter().Struct(identifier->Id()).Find();

        if (!iter)
        {
            throw "Cannot find struct in this scope";
        }

        table.EnterScope(iter);
        Ref<TypeSpecifier> typeSpecifier;
        bool isPublic = false;
        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
        {
            IdentifierExpression *identifierExpression = m_Property->Into<IdentifierExpression>();
            typeSpecifier = AnalyzeIdentifier(identifierExpression, isPublic);
            break;
        }
        case ExpressionType::FunctionCall:
        {
            FunctionCallExpression *fnCallExpression = m_Property->Into<FunctionCallExpression>();
            typeSpecifier = AnalyzeMethod(fnCallExpression, isPublic);
            break;
        }
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        {
            table.LeaveScope();
            return m_Property->Analyze();
        }
        default:
        {
            table.LeaveScope();
            throw "Invalid property expression";
        }
        }
        table.LeaveScope();
        if (isPublic)
        {
            return typeSpecifier;
        }

        if (auto parentNode = table.CurrentScope()->Iter().Parent().Find(); parentNode)
        {
            if (parentNode != iter)
            {
                throw "Property is private";
            }
        }
        return typeSpecifier;
    }

    void MemberAccessExpression::CheckObjectExpressionType()
    {
        switch (m_Object->ExpressionType())
        {
        case ExpressionType::Binary:
        case ExpressionType::Assignment:
        case ExpressionType::Cast:
        case ExpressionType::ArrayInit:
        case ExpressionType::ObjectInit:
        case ExpressionType::NameSpace:
        case ExpressionType::Primitive:
        case ExpressionType::Unary:
        case ExpressionType::MemberAccess:
            throw "Invalid object expression";
        default:
            break;
        }
    }

    Ref<TypeSpecifier> MemberAccessExpression::AnalyzeMethod(FunctionCallExpression *fnCallExpression, bool &isPublic)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        const IdentifierExpression *identifierExpression = fnCallExpression->FnName().TryInto<IdentifierExpression>();
        if (!identifierExpression)
        {
            throw "Function name must be identifier expression";
        }
        SymbolNode *fnNode = structSymbol->Iter()
                                 .Function(identifierExpression->GetIdentifier().Id())
                                 .Find();
        if (!fnNode)
        {
            throw "Cannot find method";
        }

        isPublic = CheckAccessSpecifier(fnNode->GetSymbolData().Access());
        return fnCallExpression->Analyze();
    }

    Ref<TypeSpecifier> MemberAccessExpression::AnalyzeIdentifier(IdentifierExpression *identifierExpression, bool &isPublic)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        SymbolNode *fieldNode = structSymbol->Iter()
                                    .StructField(identifierExpression->GetIdentifier().Id())
                                    .Find();
        if (!fieldNode)
        {
            throw "Cannot find member";
        }
        isPublic = CheckAccessSpecifier(fieldNode->GetSymbolData().Access());
        VariableSymbolNode *variableNode = fieldNode->Into<VariableSymbolNode>();
        return variableNode->GetTypeSpecifier();
    }

    bool MemberAccessExpression::CheckAccessSpecifier(SymbolAccess access)
    {
        return access == SymbolAccess::Public;
    }

} // namespace Marble
