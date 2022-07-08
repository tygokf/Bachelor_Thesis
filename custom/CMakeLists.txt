
SET(BuildClassFunctionsOnlyRule ClassFunctionsOnly)
BUILD_DYNAMIC_RULES("${BuildClassFunctionsOnlyRule}")

SET(BuildDeclareConstRule DeclareConst)
BUILD_DYNAMIC_RULES("${BuildDeclareConstRule}")

SET(BuildGlobalVariableRule GlobalVariable)
BUILD_DYNAMIC_RULES("${BuildGlobalVariableRule}")

SET(BuildIllegalClassMethodRule IllegalClassMethod)
BUILD_DYNAMIC_RULES("${BuildIllegalClassMethodRule}")

SET(BuildInitVariableRule InitVariable)
BUILD_DYNAMIC_RULES("${BuildInitVariableRule}")

SET(BuildLongMethodRule LongMethod)
BUILD_DYNAMIC_RULES("${BuildLongMethodRule}")

SET(BuildMostLocalScopeRule MostLocalScope)
BUILD_DYNAMIC_RULES("${BuildMostLocalScopeRule}")

SET(BuildNamingConventionRule NamingConvention)
BUILD_DYNAMIC_RULES("${BuildNamingConventionRule}")

SET(BuildNoArrayRule NoArray)
BUILD_DYNAMIC_RULES("${BuildNoArrayRule}")

SET(BuildNoStructRule NoStruct)
BUILD_DYNAMIC_RULES("${BuildNoStructRule}")

SET(BuildRecursionRule Recursion)
BUILD_DYNAMIC_RULES("${BuildRecursionRule}")

SET(BuildTooLittleClassesRule TooLittleClasses)
BUILD_DYNAMIC_RULES("${BuildTooLittleClassesRule}")

SET(BuildTooLittleFunctionsRule TooLittleFunctions)
BUILD_DYNAMIC_RULES("${BuildTooLittleFunctionsRule}")

SET(BuildUnusedLocalVariableRule UnusedLocalVariable)
BUILD_DYNAMIC_RULES("${BuildUnusedLocalVariableRule}")

SET(BuildUnusedMethodParameterRule UnusedMethodParameter)
BUILD_DYNAMIC_RULES("${BuildUnusedMethodParameterRule}")

SET(BuildConsistantBracesRule ConsistantBraces)
BUILD_DYNAMIC_RULES("${BuildConsistantBracesRule}")

SET(BuildEmptyLineRule EmptyLine)
BUILD_DYNAMIC_RULES("${BuildEmptyLineRule}")

SET(BuildIndentationRule Indentation)
BUILD_DYNAMIC_RULES("${BuildIndentationRule}")

SET(BuildNamespaceRule Namespace)
BUILD_DYNAMIC_RULES("${BuildNamespaceRule}")

SET(BuildWhiteSpaceRule WhiteSpace)
BUILD_DYNAMIC_RULES("${BuildWhiteSpaceRule}")
