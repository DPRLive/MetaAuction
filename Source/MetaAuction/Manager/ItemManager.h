// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/ActorComponent.h> 
#include "ItemManager.generated.h"

class AItemActor;
class FJsonObject;

/**
 *  물품 ID에 대한 데이터
 */
USTRUCT()
struct FItemData
{
	GENERATED_BODY()

	FItemData() : ItemID(0), SellerName(TEXT("")), BuyerName(TEXT("")), Title(TEXT("")), Information(TEXT("")),
	Location(0), World(TEXT("")), Type(EItemDealType::None), ImgCount(0), StartPrice(0), CurrentPrice(0), HaveGlb(false) {}

	// 물품 ID
	UPROPERTY()
	uint32 ItemID;
	
	// 판매자 이름
	UPROPERTY()
	FString SellerName;

	// 구매자 이름
	UPROPERTY()
	FString BuyerName;
	
	// 물품 제목
	UPROPERTY()
	FString Title;

	// 물품 상세 정보
	UPROPERTY()
	FString Information;

	// 위치
	UPROPERTY()
	uint8 Location;

	// 어디서 판매중인지 web : 웹에서만, 메타버스에서도 판매중이면 월드 번호
	UPROPERTY()
	FString World;

	// 판매 타입이 무엇인지, auction or normal
	UPROPERTY()
	EItemDealType Type;
	
	// 이미지 파일의 갯수
	UPROPERTY()
	uint8 ImgCount;
	
	// 물품 경매 시작 가격
	UPROPERTY()
	uint64 StartPrice;

	// 물품 경매 현재 가격
	UPROPERTY()
	uint64 CurrentPrice;

	// 물품 경매 현재 가격
	UPROPERTY()
	uint8 HaveGlb:1;
	
	// 종료 시각. 배열에 순서대로 년 / 월 / 일 / 시간 / 분 
	UPROPERTY()
	TArray<uint16> EndTime;
};

/**
 *  물품 검색 시 사용할 Option struct
 *  필요한 옵션만 넣어서 사용하면 됩니다.
 */
USTRUCT()
struct FItemSearchOption
{
	GENERATED_BODY()

	FItemSearchOption() : SearchString(TEXT("")), World(TEXT("")), ItemType(EItemDealType::None), CanDeal(EItemCanDeal::None) {}
	
	// 제목 또는 내용에 해당 문자열이 포함되어 있는지 확인
	FString SearchString;

	// 어떤 월드에 해당하는 물품인지
	FString World;

	// 판매 타입이 어떤것인지
	EItemDealType ItemType;

	// 현재 판매중인(구매가 가능한) 상품인지
	EItemCanDeal CanDeal;
};

/**
 *  전체적인 물품들의 상태를 관리해주는 Item Manager
 *  GameState에서 관리하여 물품에 대한 관리는 server가 주도함.
 *  TODO: 물품 관리에 대한 쿼리용 함수 작성
 *  TODO : 아이템 변경 관련한 코드 작성 (지금은 websocket->dedi->client에서 broadcast 생각중
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAAUCTION_API UItemManager : public UActorComponent
{
	GENERATED_BODY()

	// GetItemData에 넣을때 쓸 람다 타입
	using FGetItemDataByIdCallback = TFunction<void(const FItemData&)>;
	using FGetItemDataByOptionCallback = TFunction<void(const TArray<FItemData>&)>;

public:	
	UItemManager();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 새로운 상품이 등록되면, 서버에게 ItemData 요청 후 내 월드에 맞는 상품이면 배치합니다. 
	// 데디 서버에서만 실행 가능합니다.
	void Server_RegisterNewItem(uint32 InItemId);

	// 웹서버에 등록된 현재 월드에 배치되어 판매되고 있는 아이템들의 ID를 가져와 배치한다.
	// 데디 서버에서만 실행 가능합니다.
	void Server_RegisterAllWorldItemID();

	// ItemID로 물품 정보를 요청합니다. 
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void GetItemDataByID(FGetItemDataByIdCallback InFunc, uint32 InItemId);

	// 옵션을 걸어 물품 정보들을 요청합니다.
	// 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
	void GetItemDataByOption(FGetItemDataByOptionCallback InFunc, const FItemSearchOption& InSearchOption);

	// HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
	void Client_RequestBid(uint32 InItemID, uint64 InPrice);
protected:
	virtual void BeginPlay() override;

private:
	// Item Data의 Location을 기준으로, Item Actor에 상품ID를 등록한다.
	// 데디 서버에서만 실행 가능합니다.
	void _Server_RegisterItemByLoc(uint32 InItemId, uint8 InItemLoc);
	
	// Json 형태의 ItemData JsonObejct를 FItemData로 변환한다.
	void _JsonToItemData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const;
	
	// 물품을 배치할 수 있는 ItemActor에 대한 포인터들. 사용자에게 표시는 1부터 할거지만 접근은 인덱스 0부터 해주세요.
	// 클라이언트들에서도 읽을 수 있습니다.
	UPROPERTY( Replicated )
	TArray<TWeakObjectPtr<AItemActor>> ItemActors;
};
