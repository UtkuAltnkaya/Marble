#include "Ast/Expressions.hpp"
#include "SymbolTable/SymbolTable.hpp"
#include "SemanticAnalyzer/SemanticAnalyzer.hpp"
#include "ErrorSystem/ErrorSystem.hpp"
#include "Utils/IDGenerator.hpp"

namespace Marble
{
    Ref<TypeSpecifier> MemberAccessExpression::Analyze(SemanticAnalyzer &semanticAnalyzer)
    {
        Ref<TypeSpecifier> objectType = AnalyzeObject(semanticAnalyzer);
        const Identifier &structName = FindStructName(semanticAnalyzer, objectType);
        StructSymbolNode *structNode = SymbolIterator().Struct(structName.Id());
        if (!structNode)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find struct in this scope", true);
        }
        BlockSymbolNode *properties = structNode->Block();
        bool isPublic = AnalyzeProperty(semanticAnalyzer, properties, structNode);
        return CheckVisibility(semanticAnalyzer, isPublic);
    }

    Ref<TypeSpecifier> MemberAccessExpression::AnalyzeObject(SemanticAnalyzer &semanticAnalyzer)
    {
        switch (m_Object->ExpressionType())
        {
        case ExpressionType::FunctionCall:
        case ExpressionType::Identifier:
        case ExpressionType::ArrayIndex:
        case ExpressionType::NameSpace:
        case ExpressionType::MemberAccess:
            break;
        default:
            ErrorSystem::AddError(semanticAnalyzer, this, "Invalid object expression", true);
            break;
        }
        Ref<TypeSpecifier> objectType = m_Object->Analyze(semanticAnalyzer);
        bool constFlag = objectType->GetType() == Types::ConstantType;
        return TypeSpecifier::PassConst(objectType);
    }

    const Identifier &MemberAccessExpression::FindStructName(SemanticAnalyzer &semanticAnalyzer, Ref<TypeSpecifier> objectType)
    {

        if (objectType->GetType() == Types::UserDefine)
        {
            if (m_AccessType != TokenType::Dot)
            {
                ErrorSystem::AddError(semanticAnalyzer, this, "Use dot('.') operator to access member");
            }
            return objectType->UserDefine();
        }

        if (objectType->GetType() == Types::Pointer)
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
            return pointerType->UserDefine();
        }

        ErrorSystem::AddError(semanticAnalyzer, this, "Member access only can use with user define type", true);
        UNREACHABLE();
    }

    bool MemberAccessExpression::AnalyzeProperty(
        SemanticAnalyzer &semanticAnalyzer, BlockSymbolNode *properties, StructSymbolNode *structNode)
    {
        bool isPublic = false;
        switch (m_Property->ExpressionType())
        {
        case ExpressionType::Identifier:
            AnalyzeIdentifier(semanticAnalyzer, properties, isPublic);
            break;
        case ExpressionType::FunctionCall:
            AnalyzeMethod(semanticAnalyzer, structNode, isPublic);
            break;
        default:
            ErrorSystem::AddError(semanticAnalyzer, this, "Invalid property expression", true);
            break;
        }
        return isPublic;
    }

    void MemberAccessExpression::AnalyzeIdentifier(SemanticAnalyzer &semanticAnalyzer, BlockSymbolNode *properties, bool &isPublic)
    {
        const IdentifierExpression *identifier = m_Property->Into<IdentifierExpression>();
        VariableSymbolNode *field = SymbolIterator(properties).Variable(identifier->GetIdentifier().Id());
        if (!field)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find property of the struct", true);
        }
        m_ValueType = field->GetTypeSpecifier();
        isPublic = field->GetSymbolData().Access() == SymbolAccess::Public;
    }

    void MemberAccessExpression::AnalyzeMethod(SemanticAnalyzer &semanticAnalyzer, StructSymbolNode *structNode, bool &isPublic)
    {
        FunctionCallExpression *fnCall = m_Property->Into<FunctionCallExpression>();
        IdentifierExpression *identifier = fnCall->FnName().TryInto<IdentifierExpression>();
        if (!identifier)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Function name must be identifier expression", true);
        }
        auto fnName = structNode->LookFunctionName(identifier->GetIdentifier().Id());
        if (!fnName)
        {
            ErrorSystem::AddError(semanticAnalyzer, this, "Cannot find related function name with given method name", true);
        }
        identifier->SetId(*fnName);
        Box<Expression> pointerToStruct = CreateObjectPointer(structNode);
        fnCall->AddArg(std::move(pointerToStruct), 0);
        m_ValueType = fnCall->Analyze(semanticAnalyzer);
        isPublic = SymbolIterator().Function(*fnName)->GetSymbolData().Access() == SymbolAccess::Public;
    }

    Box<Expression> MemberAccessExpression::CreateObjectPointer(StructSymbolNode *structNode)
    {
        auto iter = m_Object.get();
        Marble::ExpressionType objectType = iter->ExpressionType();
        Box<Expression> firstArg;
        if (objectType == ExpressionType::MemberAccess)
        {
            Identifier tempName("temp_" + structNode->GetSymbolData().Name() + "_" + IDGenerator::Generate(), Span{});
            Identifier structName(structNode->GetSymbolData().Name(), Span{});
            Ref<TypeSpecifier> ts = MakeRef<TypeSpecifier>(structName, Span{});
            VariableSymbolNode *tempNode = new VariableSymbolNode(*tempName, SymbolAccess::Local, ts);
            SymbolTable::Get().CurrentScope()->Insert(tempNode);
            firstArg = MakeBox<IdentifierExpression>(std::move(tempName), Span{});
        }
        else
        {
            firstArg = m_Object->Clone();
        }
        Span span = firstArg->GetSpan();
        return MakeBox<UnaryExpression>(UnaryOperators::Address, std::move(firstArg), UnaryExpressionType::Prefix, span);
    }

    Ref<TypeSpecifier> MemberAccessExpression::CheckVisibility(SemanticAnalyzer &semanticAnalyzer, bool isPublic)
    {
        if (isPublic)
        {
            return m_ValueType;
        }

        FunctionSymbolNode *parentFnNode = SymbolIterator(SymbolTable::Get().CurrentScope()).Function();
        ASSERT_D(parentFnNode != nullptr, "Cannot find the parent function node");

        if (parentFnNode->IsMethod())
        {
            return m_ValueType;
        }

        ErrorSystem::AddError(semanticAnalyzer, this, "Property is private");
        return m_ValueType;
    }

    void MemberAccessExpression::SubstituteGenerics(SemanticAnalyzer &semanticAnalyzer, const std::unordered_map<std::string, Ref<TypeSpecifier>> &map)
    {
        m_Object->SubstituteGenerics(semanticAnalyzer, map);
        m_Property->SubstituteGenerics(semanticAnalyzer, map);
    }

    Box<Expression> MemberAccessExpression::Clone()
    {
        return MakeBox<MemberAccessExpression>(*this);
    }

} // namespace Marble
