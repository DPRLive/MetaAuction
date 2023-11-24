// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <UObject/Interface.h>
#include "MAPerspectiveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMAPerspectiveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 관점 변경 (1인칭 3인칭)을 위한 interface 입니다.
 */
class METAAUCTION_API IMAPerspectiveInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 시점을 변경합니다.
	virtual void ChangePerspective() = 0;
	
};
