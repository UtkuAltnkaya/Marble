#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "ErrorSystem/ErrorSystem.hpp"

namespace Marble
{
    // TODO It marks the property value as constat if is the case
    // TODO This restrict to return or create new value as const to underlying type
    // TODO It can be bypass by casting to non-const version but gives warning
    Ref<TypeSpecifier> MemberAccessExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {

        CheckObjectExpressionType(semanticAnalyzer);
        Ref<TypeSpecifier> objectType = m_Object->Analyze(semanticAnalyzer);

        bool constFlag = objectType->GetType() == Types::ConstantType;
        objectType = TypeSpecifier::PassConst(objectType);

        const Identifier *identifier = nullptr;
        if (objectType->GetType() == Types::UserDefine)
        {
            if (m_AccessType != TokenType::Dot)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Use dot('.') operator to access member");
            }
            identifier = &objectType->UserDefine();
        }
        else if (objectType->GetType() == Types::Pointer)
        {
            const PointerType &pointer = objectType->Pointer();
            Ref<TypeSpecifier> pointerType = pointer.TypeSpecifier;
            if (pointerType->GetType() != Types::UserDefine)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Member access only can use with user define type");
            }
            if (m_AccessType != TokenType::Arrow)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Use arrow('->') operator to access member with pointer type");
            }
            identifier = &pointerType->UserDefine();
        }
        else
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Member access only can use with user define type", true);
        }

        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *iter = table.Iter().Struct(identifier->Id()).Find();

        if (!iter)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find struct in this scope", true);
        }

        table.EnterScope(iter);
        Ref<TypeSpecifier> typeSpecifier = nullptr;
        bool isPublic = false;
        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
        {
            IdentifierExpression *identifierExpression = m_Property->Into<IdentifierExpression>();
            typeSpecifier = AnalyzeIdentifier(semanticAnalyzer, identifierExpression, isPublic);
            if (constFlag)
                typeSpecifier = TypeSpecifier::ConvertToConst(typeSpecifier);
            table.LeaveScope();
            break;
        }
        case ExpressionType::FunctionCall:
        {
            FunctionCallExpression *fnCallExpression = m_Property->Into<FunctionCallExpression>();
            typeSpecifier = AnalyzeMethod(semanticAnalyzer, fnCallExpression, isPublic);
            break;
        }
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        {
            typeSpecifier = m_Property->Analyze(semanticAnalyzer);
            if (constFlag)
                typeSpecifier = TypeSpecifier::ConvertToConst(typeSpecifier);
            table.LeaveScope();
            return typeSpecifier;
        }
        default:
        {
            table.LeaveScope();
            ErrorSystem::AddError(semanticAnalyzer, this, "Invalid property expression", true);
        }
        }
        if (isPublic)
        {
            return typeSpecifier;
        }

        auto parentNode = table.CurrentScope()->Iter().Parent().Find();
        while (parentNode)
        {
            if (parentNode == iter)
            {
                return typeSpecifier;
            }
            parentNode = parentNode->Iter().Parent().Find();
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Property is private");
        return typeSpecifier;
    }

    void MemberAccessExpression::CheckObjectExpressionType(SemanticAnalyzer &semanticAnalyzer)
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
            ErrorSystem::AddError(semanticAnalyzer, this, "Invalid object expression", true);
        default:
            break;
        }
    }

    Ref<TypeSpecifier> MemberAccessExpression::AnalyzeMethod(SemanticAnalyzer &semanticAnalyzer, FunctionCallExpression *fnCallExpression, bool &isPublic)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        const IdentifierExpression *identifierExpression = fnCallExpression->FnName().TryInto<IdentifierExpression>();
        if (!identifierExpression)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Function name must be identifier expression", true);
        }
        SymbolNode *fnNode = structSymbol->Iter()
                                 .Function(identifierExpression->GetIdentifier().Id())
                                 .Find();
        if (!fnNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find method", true);
        }

        isPublic = CheckAccessSpecifier(fnNode->GetSymbolData().Access());
        return fnCallExpression->Analyze(semanticAnalyzer);
    }

    Ref<TypeSpecifier> MemberAccessExpression::AnalyzeIdentifier(SemanticAnalyzer &semanticAnalyzer, IdentifierExpression *identifierExpression, bool &isPublic)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        SymbolNode *fieldNode = structSymbol->Iter()
                                    .StructField(identifierExpression->GetIdentifier().Id())
                                    .Find();
        if (!fieldNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find member", true);
        }
        isPublic = CheckAccessSpecifier(fieldNode->GetSymbolData().Access());
        VariableSymbolNode *variableNode = fieldNode->Into<VariableSymbolNode>();
        return variableNode->GetTypeSpecifier();
    }

    void MemberAccessExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Object->SubstituteGenerics(semanticAnalyzer, map);
        m_Property->SubstituteGenerics(semanticAnalyzer, map);
    }

    bool MemberAccessExpression::CheckAccessSpecifier(SymbolAccess access)
    {
        return access == SymbolAccess::Public;
    }

    Box<Expression> MemberAccessExpression::Clone()
    {
        return MakeBox<MemberAccessExpression>(*this);
    }
} // namespace Marble
