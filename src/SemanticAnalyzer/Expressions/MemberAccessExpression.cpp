#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Utils/IDGenerator.hpp"

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
        bool isPublic = false;
        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
        {
            IdentifierExpression *identifierExpression = m_Property->Into<IdentifierExpression>();
            m_ValueType = AnalyzeIdentifier(semanticAnalyzer, identifierExpression, isPublic);
            if (constFlag)
                m_ValueType = TypeSpecifier::ConvertToConst(m_ValueType);
            table.LeaveScope();
            break;
        }
        case ExpressionType::FunctionCall:
        {
            FunctionCallExpression *fnCallExpression = m_Property->Into<FunctionCallExpression>();
            m_ValueType = AnalyzeMethod(semanticAnalyzer, fnCallExpression, identifier, isPublic);
            break;
        }
        case ExpressionType::MemberAccess:
        case ExpressionType::ArrayIndex:
        {
            m_ValueType = m_Property->Analyze(semanticAnalyzer);
            if (constFlag)
                m_ValueType = TypeSpecifier::ConvertToConst(m_ValueType);
            return m_ValueType;
        }
        default:
        {
            table.LeaveScope();
            ErrorSystem::AddError(semanticAnalyzer, this, "Invalid property expression", true);
        }
        }
        if (isPublic)
        {
            return m_ValueType;
        }

        auto parentNode = table.CurrentScope()->Iter().Parent().Find();
        while (parentNode)
        {
            if (parentNode == iter)
            {
                return m_ValueType;
            }
            parentNode = parentNode->Iter().Parent().Find();
        }
        ErrorSystem::AddError(semanticAnalyzer, this, "Property is private");
        return m_ValueType;
    }

    void MemberAccessExpression::CheckObjectExpressionType(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_Object->ExpressionType())
        {

        case ExpressionType::FunctionCall:
        case ExpressionType::Identifier:
        case ExpressionType::ArrayIndex:
        case ExpressionType::NameSpace:
            break;
        default:
            ErrorSystem::AddError(semanticAnalyzer, this, "Invalid object expression", true);
            break;
        }
    }

    Ref<TypeSpecifier> MemberAccessExpression::AnalyzeMethod(
        SemanticAnalyzer &semanticAnalyzer, FunctionCallExpression *fnCallExpression, const Identifier *structName, bool &isPublic)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        SymbolNode *structSymbol = table.CurrentScope();
        IdentifierExpression *identifierExpression = fnCallExpression->FnName().TryInto<IdentifierExpression>();
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
        Ref<TypeSpecifier> ts = fnCallExpression->Analyze(semanticAnalyzer);
        DeSugar(semanticAnalyzer, fnCallExpression, structName, fnNode, structSymbol);
        return ts;
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

    void MemberAccessExpression::DeSugar(
        SemanticAnalyzer &semanticAnalyzer, FunctionCallExpression *fnCallExpression, const Identifier *structName, SymbolNode *fnNode, SymbolNode *currentScope)
    {
        SymbolTable &table = SymbolTable::GetInstance();
        IdentifierExpression *identifierExpression = fnCallExpression->FnName().Into<IdentifierExpression>();
        const std::string &fnDefName = semanticAnalyzer.ConvertMethodIntoFunction(static_cast<Definition *>(fnNode->GetAstPtr()), structName->Id(), currentScope);

        Box<Expression> firstArg;

        switch (m_Object->ExpressionType())
        {
        case ExpressionType::FunctionCall:
        case ExpressionType::NameSpace:
        {
            SymbolNode *node = table.CurrentScope();
            Identifier tempName{"temp_" + structName->Id() + "_" + IDGenerator::Generate(), Span{}};
            Ref<TypeSpecifier> ts = MakeRef<TypeSpecifier>(*structName, Span{});
            VariableSymbolNode *variableNode = new VariableSymbolNode{tempName, node, ts, SymbolNodeTypes::Variable};
            variableNode->SetTempVariable(true);
            node->Insert(tempName.Id(), variableNode);
            firstArg = MakeBox<IdentifierExpression>(std::move(tempName), Span{});
            break;
        }
        default:
        {
            firstArg = m_Object->Clone();
            break;
        }
        }

        identifierExpression->SetId(fnDefName);
        Span span = firstArg->GetSpan();
        m_TempValue = firstArg->Clone();
        Box<Expression> pointerToStruct = MakeBox<UnaryExpression>(UnaryOperators::Address, std::move(firstArg), UnaryExpressionType::Prefix, span);
        fnCallExpression->AddArg(std::move(pointerToStruct), 0);
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
