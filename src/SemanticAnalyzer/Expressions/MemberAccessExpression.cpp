#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"

namespace Marble
{
    static void CheckObjectExpressionType(ExpressionType expressionType);
    static Ref<TypeSpecifier> AnalyzeMethod(FunctionCallExpression *fnCallExpression);
    static Ref<TypeSpecifier> AnalyzeIdentifier(IdentifierExpression *identifierExpression);
    static void CheckAccessSpecifier(SymbolAccess access);

    Ref<TypeSpecifier> MemberAccessExpression::Analyze()
    {
        CheckObjectExpressionType(m_Object->ExpressionType());
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
        SymbolNode *iter = table.Iter().Struct(identifier->Id()).Ok().Find();

        table.EnterScope(iter);
        Ref<TypeSpecifier> typeSpecifier;
        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
        {
            IdentifierExpression *identifierExpression = m_Property->Into<IdentifierExpression>();
            typeSpecifier = AnalyzeIdentifier(identifierExpression);
            break;
        }
        case ExpressionType::FunctionCall:
        {
            FunctionCallExpression *fnCallExpression = m_Property->Into<FunctionCallExpression>();
            typeSpecifier = AnalyzeMethod(fnCallExpression);
            break;
        }
        case ExpressionType::MemberAccess:
        {
            typeSpecifier = m_Property->Analyze();
        }
        default:
        {
            table.LeaveScope();
            throw "Invalid property expression";
        }
        }
        table.LeaveScope();
        return TypeSpecifierOk;
    }

    void CheckObjectExpressionType(ExpressionType expressionType)
    {
        switch (expressionType)
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

    Ref<TypeSpecifier> AnalyzeMethod(FunctionCallExpression *fnCallExpression)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        SymbolNode *fnNode = structSymbol->Iter()
                                 .Function(fnCallExpression->FnName().TryInto<IdentifierExpression>()->GetIdentifier().Id())
                                 .Ok()
                                 .Find();
        CheckAccessSpecifier(fnNode->GetSymbolData().Access());
        return fnCallExpression->Analyze();
    }

    Ref<TypeSpecifier> AnalyzeIdentifier(IdentifierExpression *identifierExpression)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        SymbolNode *fieldNode = structSymbol->Iter()
                                    .StructField(identifierExpression->GetIdentifier().Id())
                                    .Ok()
                                    .Find();
        CheckAccessSpecifier(fieldNode->GetSymbolData().Access());
        return identifierExpression->Analyze();
    }

    void CheckAccessSpecifier(SymbolAccess access)
    {
        if (access == SymbolAccess::Public)
        {
            return;
        }
        throw "Property is private";
    }

} // namespace Marble
