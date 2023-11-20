// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MAWidgetHelperLibrary.h"
#include "Handler/ItemFileHandler.h"

#include <Components/Image.h>

void UMAWidgetHelperLibrary::RequestImageByItemID(UImage* ImageWidget, uint8 ItemID, uint8 InImageNumber)
{
	TWeakObjectPtr<UImage> ImagePtr = MakeWeakObjectPtr(ImageWidget);
	if (ImagePtr.IsValid())
	{
		if (FItemFileHandler* ItemFileHandler = MAGetItemFileHandler(MAGetGameInstance(ImagePtr->GetWorld())))
		{
			auto Func = [ImagePtr](UTexture2DDynamic* InImage)
				{
					if (ImagePtr.IsValid())
					{
						ImagePtr->SetBrushFromTextureDynamic(InImage);
					}
				};

			// 0번 인덱스 이미지는 없으므로 1번 인덱스 이미지를 가져옵니다.
			ItemFileHandler->RequestImg(Func, ItemID, InImageNumber);
		}
	}
}
