// Fill out your copyright notice in the Description page of Project Settings.


#include "Handler/ItemDataHandler.h"

#include <Interfaces/IHttpResponse.h>
#include <Serialization/JsonSerializer.h>
#include <IStompMessage.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(ItemDataHandler)

UItemDataHandler::UItemDataHandler()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void UItemDataHandler::BeginPlay()
{
	Super::BeginPlay();

	if(IsRunningDedicatedServer())
	{
		// 서버에서 WebSocket에 구독할 것들을 구독한다.
		if(const FStompHelper* stompHandler = MAGetStompHelper(GetOwner()->GetGameInstance()))
		{
			// 아이템 가격 변동 구독, 이건 구독 해제할 일이 없음
			FStompSubscriptionEvent Server_EventChangePrice;
			Server_EventChangePrice.BindUObject(this, &UItemDataHandler::_Server_OnChangePrice);
			stompHandler->Subscribe(DA_NETWORK(WSChangePriceAddURL), Server_EventChangePrice);

			// 아이템 정보 변동 구독, 이건 구독 해제할 일이 없음
			FStompSubscriptionEvent Server_EventChangeItemData;
			Server_EventChangeItemData.BindUObject(this, &UItemDataHandler::_Server_OnChangeItemData);
			stompHandler->Subscribe(DA_NETWORK(WSChangeDataAddURL), Server_EventChangeItemData);
		}
	}
}

/**
 * 클라이언트에서 ItemID로 물품 정보를 요청합니다.
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 람다 함수 
 * @param InItemId : 쿼리할 물품의 ID
 */
void UItemDataHandler::RequestItemDataById(const FCallbackRefOneParam<FItemData>& InFunc, uint32 InItemId) const
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance()))
	{
		FString RequestUrl = DA_NETWORK(ItemInfoAddURL) + FString::Printf(TEXT("/%d"), InItemId);

		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<const UItemDataHandler> thisPtr = this;
		httpHelper->Request(RequestUrl, EHttpRequestType::GET, [thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
		{
			if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
			{
				const TSharedPtr<FJsonObject> jsonObject = UtilJson::StringToJson(InResponse->GetContentAsString());
				
				FItemData itemData;
				thisPtr->_JsonToData(jsonObject, itemData);

				if(InFunc) // 함수 실행
				{
					InFunc(itemData);
				}
				
				LOG_N(TEXT("Get Item Data(item id : %d) Success!"), itemData.ItemID);
			}
		});
	}
}

/**
 * 옵션을 걸어 물품 정보들을 요청합니다.
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수 
 * @param InSearchOption : 물품 검색 시 사용할 옵션, 필요한 옵션만 설정 후 넣어서 사용하면 됩니다.
 */
void UItemDataHandler::RequestItemDataByOption(const FCallbackRefArray<FItemData>& InFunc, const FItemSearchOption& InSearchOption) const
{
	// 옵션 설정
	TSharedRef<FJsonObject> requestObj = MakeShared<FJsonObject>();
	if(InSearchOption.SearchString != TEXT(""))
		requestObj->SetStringField(TEXT("searchString"), InSearchOption.SearchString.ToLower());
	
	if(InSearchOption.World != TEXT(""))
		requestObj->SetStringField(TEXT("world"), InSearchOption.World.ToLower());

	if(InSearchOption.ItemType != EItemDealType::None)
		requestObj->SetStringField(TEXT("type"), ENUM_TO_STRING(EItemDealType, InSearchOption.ItemType).ToLower());

	if(InSearchOption.CanDeal == EItemCanDeal::Possible)
		requestObj->SetBoolField(TEXT("isPossible"), true);
	else if(InSearchOption.CanDeal == EItemCanDeal::Impossible)
		requestObj->SetBoolField(TEXT("isPossible"), false);
	
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<const UItemDataHandler> thisPtr = this;
		httpHelper->Request(DA_NETWORK(ItemSearchAddURL), EHttpRequestType::POST,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 TArray<TSharedPtr<FJsonObject>> jsonArray;
									 UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

									 TArray<FItemData> itemDatas;
									 for (const TSharedPtr<FJsonObject>& itemInfo : jsonArray)
									 {
										 itemDatas.Emplace(FItemData());
										 thisPtr->_JsonToData(itemInfo, *itemDatas.rbegin());
									 }

									 if (InFunc)
									 {
										 InFunc(itemDatas);
									 }
								 	
									 LOG_N(TEXT("Search Items Success!"));
								 }
							 }, UtilJson::JsonToString(requestObj));
	}
}

/**
* HTTP 통신으로 웹서버에 입찰을 요청하는 함수입니다.
* @param InItemId : 입찰할 item의 id
* @param InPrice : 입찰할 가격
*/
void UItemDataHandler::Client_RequestBid(uint32 InItemId, uint64 InPrice) const
{
	if (IsRunningDedicatedServer()) return; // 데디면 하지 마라.

	// Body를 만든다.
	FString requestBody = FString::Printf(TEXT("%llu"), InPrice);

	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance())) // 로컬에 없으니 다운로드를 함
	{
		httpHelper->Request(DA_NETWORK(BidAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::POST,
		                     [](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
		                     {
			                     if (InbWasSuccessful && InResponse.IsValid())
			                     {
				                     if (EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
				                     {
					                     // 입찰 성공
					                     LOG_WARN(TEXT("Bid Success!"));
				                     }
				                     else
				                     {
					                     // 입찰 실패, InResponse->GetContentAsString() : 서버에서 알려준 입찰 실패 이유
					                     LOG_WARN(TEXT("bid Failed!"));
					                     // Json reader 생성
					                     UE_LOG(LogTemp, Warning, TEXT("%s"), *InResponse->GetContentAsString());
				                     }
			                     }
		                     }, requestBody);
	}
}

/**
 * ItemID로 물품에 대한 입찰 기록을 조회합니다. DB에서 입찰 순서대로 내림차순해서 정렬하여 주니, 그대로 배열을 사용하시면 됩니다.
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수 
 * @param InItemId : 입찰 기록을 조회할 상품의 ID
 */
void UItemDataHandler::RequestBidRecordByItemId(const FCallbackRefArray<FBidRecord>& InFunc, uint32 InItemId) const
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<const UItemDataHandler> thisPtr = this;
		httpHelper->Request(DA_NETWORK(BidRecordAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 TArray<TSharedPtr<FJsonObject>> jsonArray;
									 UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);
								 	
									 TArray<FBidRecord> BidRecords;
									 for (const TSharedPtr<FJsonObject>& bidInfo : jsonArray)
									 {
										 BidRecords.Emplace(FBidRecord());
										 thisPtr->_JsonToData(bidInfo, *BidRecords.rbegin());
									 }

									 if (InFunc)
									 {
										 InFunc(BidRecords);
									 }
								 	
									 LOG_N(TEXT("Get Bid Records Success!"));
								 }
							 });
	}
}

/**
 * ItemId로 물품을 삭제합니다. (JWT 토큰 확인으로 내 물품이 맞을 경우만 삭제함, 판매 종료 3시간 전에는 삭제 불가)
 * @param InItemId : 삭제할 상품의 ID
 */
void UItemDataHandler::RequestRemoveItem(uint32 InItemId) const
{
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance()))
	{
		httpHelper->Request(DA_NETWORK(RemoveItemAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::POST,[](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
								 	if (InbWasSuccessful && InResponse.IsValid())
								 	{
										 if (EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
										 {
											 // 삭제 성공
											 LOG_WARN(TEXT("Remove Success!"));
										 }
										 else
										 {
											 // 삭제 실패, InResponse->GetContentAsString() : 서버에서 알려준 삭제 실패 이유
											 LOG_WARN(TEXT("Remove Item Failed!"));
											 // Json reader 생성
											 UE_LOG(LogTemp, Warning, TEXT("%s"), *InResponse->GetContentAsString());
										 }
									 }
								 }
							 });
	}
}

/**
 * Type : Sell (내가 판매했던 or 판매 중인 or 판매했는데 입찰 실패한 물품을 요청합니다. 최근에 업로드한 물품이 배열의 앞) 
 * Type : Buy (내가 구매 성공한 물품을 요청합니다. 최근에 업로드한 물품이 배열의 앞)
 * Type : TryBid (현재 판매중인 물품이면서, 내가 입찰을 시도 했던 물품. 최근 입찰한 것이 배열 앞)
 * (로그인 된 상태여야함)
 * 웹에 정보를 새로 요청하는 구조이므로 도착하면 실행할 함수를 Lambda로 넣어주세요. this 캡처시 weak capture로 꼭 생명주기 체크를 해야합니다!
 * @param InFunc : 정보가 도착하면 실행할 FCallbackRefArray 형태의 함수
 * @param InMyItemReqType : 어떠한 내 아이템을 요청할 것인지 설정합니다. 
 */
void UItemDataHandler::RequestMyItem(const FCallbackRefArray<FItemData>& InFunc, EMyItemReqType InMyItemReqType) const
{
	FString reqAddURL;
	if(InMyItemReqType == EMyItemReqType::Buy)
		reqAddURL = DA_NETWORK(MyBuyItemAddURL);
	else if(InMyItemReqType == EMyItemReqType::Sell)
		reqAddURL = DA_NETWORK(MySellItemAddURL);
	else if(InMyItemReqType == EMyItemReqType::TryBid)
		reqAddURL = DA_NETWORK(MyBidItemAddURL);
	
	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance()))
	{
		// 혹시 모르니 나를 잘 가리키고 있는지 확인하기 위해 weak 캡처 추가.
		TWeakObjectPtr<const UItemDataHandler> thisPtr = this;
		httpHelper->Request(reqAddURL, EHttpRequestType::GET,[thisPtr, InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (thisPtr.IsValid() && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 TArray<TSharedPtr<FJsonObject>> jsonArray;
									 UtilJson::StringToJsonValueArray(InResponse->GetContentAsString(), jsonArray);

									 TArray<FItemData> itemDatas;
									 for (const TSharedPtr<FJsonObject>& itemInfo : jsonArray)
									 {
										 itemDatas.Emplace(FItemData());
										 thisPtr->_JsonToData(itemInfo, *itemDatas.rbegin());
									 }

									 if (InFunc)
									 {
										 InFunc(itemDatas);
									 }
								 	
									 LOG_N(TEXT("Get My Items Success!"));
								 }
							 });
	}
}

/**
 *  물품의 판매자가 맞는지 JwtToken을 통해 검증하는 함수입니다.
 *  @param InItemId : 검증할 아이템 id
 *  @param InJwtToken : 검증할 유저의 토큰
 *  @param InFunc : 검증 된 후 실행할 함수
 */
void UItemDataHandler::Server_ValidateItem(const uint32 InItemId, const FString& InJwtToken, const FCallbackOneParam<bool>& InFunc) const
{
	CHECK_DEDI_FUNC;

	if (const FHttpHelper* httpHelper = MAGetHttpHelper(GetOwner()->GetGameInstance()))
	{
		httpHelper->Request(DA_NETWORK(ValidateItemAddURL) + FString::Printf(TEXT("/%d"), InItemId), EHttpRequestType::GET,
			[InFunc](FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool InbWasSuccessful)
							 {
								 if (InFunc && InbWasSuccessful && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode()))
								 {
									 // true인지 확인
									 bool isMine = InResponse->GetContentAsString().Contains(TEXT("true"));
									 InFunc(isMine);
								 }
							 }, TEXT(""), InJwtToken);
	}
}

/**
 *  Stomp 메세지로 온 아이템 가격 변동 알림을 받는다.
 */
void UItemDataHandler::_Server_OnChangePrice(const IStompMessage& InMessage) const
{
	CHECK_DEDI_FUNC;

	TArray<FString> idAndPrice;
	InMessage.GetBodyAsString().ParseIntoArray(idAndPrice, TEXT("-"), true);

	if(idAndPrice.Num() == 2) // ItemID. 가격. 딱 두개 정확히 왔을때 Multicast
	{
		_MulticastRPC_ChangePrice(FCString::Atoi(*idAndPrice[0]), FCString::Atoi64(*idAndPrice[1]));
	}
}

/**
 *  Stomp 메세지로 아이템 정보 변동 알림을 받는다.
 *  무조건 "(itemid)&(바뀌기전 월드)" 로 시작
 */
void UItemDataHandler::_Server_OnChangeItemData(const IStompMessage& InMessage) const
{
	CHECK_DEDI_FUNC;
	
	// 응답을 두개로 분할한다
	FString message = InMessage.GetBodyAsString();
	FString idAndWorld;
	FString changeList;
	if(!message.Split(TEXT("-"), &idAndWorld, &changeList))
		return;
	
	// 응답의 첫 시작인 itemId, 바뀌기 전 월드 두개를 뽑아낸다.
	FString itemId;
	FString world;
	if(!idAndWorld.Split(TEXT("&"), &itemId, &world))
		return;
	
	_MulticastRPC_ChangeItemData(FCString::Atoi(*itemId), world, changeList);
}

/**
 * 서버 -> RPC로 모든 클라에게 가격 변동 알림을 줍니다.
 * @param InItemId : 가격이 변동된 상품의 ID
 * @param InPrice : 현재 가격
 */
void UItemDataHandler::_MulticastRPC_ChangePrice_Implementation(const uint32& InItemId, const uint64& InPrice) const
{
	LOG_WARN(TEXT("id[%d] change price : %lld"), InItemId, InPrice);

	if(!IsRunningDedicatedServer()) // 근데 서버는 안궁금해
		return;
	
	if(OnChangePrice.IsBound())
		OnChangePrice.Broadcast(InItemId, InPrice);
}

/**
 * 서버 -> RPC로 모든 클라에게 아이템 정보 변동 알림을 줍니다.
*  @param InItemId : 변경된 item ID
 *  @param InWorld : 그 물품이 있던 world
 *  @param InChangeList : 바뀐 항목에 대한 string ( -로 구분, (ex)location-world )
 */
void UItemDataHandler::_MulticastRPC_ChangeItemData_Implementation(const uint32& InItemId, const FString& InWorld, const FString& InChangeList) const
{
	LOG_WARN(TEXT("id[%d] item data changed!"), InItemId);
	
	if(OnChangeItemData.IsBound())
		OnChangeItemData.Broadcast(InItemId, InWorld, InChangeList);
}

/**
 * Json 형태의 ItemData String을 FItemData로 변환한다.
 * @param InJsonObj : Json 형태의 itemData String
 * @param OutItemData : 파싱한 정보를 담아갈 ItemData
 */
void UItemDataHandler::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FItemData& OutItemData) const
{
	InJsonObj->TryGetNumberField(TEXT("id"), OutItemData.ItemID);
	InJsonObj->TryGetStringField(TEXT("sellerUsername"), OutItemData.SellerName);
	InJsonObj->TryGetStringField(TEXT("buyerUsername"), OutItemData.BuyerName);
	InJsonObj->TryGetStringField(TEXT("title"), OutItemData.Title);
	InJsonObj->TryGetStringField(TEXT("itemInformation"), OutItemData.Information);

	FString location;
	InJsonObj->TryGetStringField(TEXT("location"), location);
	OutItemData.Location = FCString::Atoi(*location);

	InJsonObj->TryGetStringField(TEXT("world"), OutItemData.World);

	FString type;
	InJsonObj->TryGetStringField(TEXT("type"), type);
	if(type == TEXT("auction")) OutItemData.Type = EItemDealType::Auction;
	else if(type == TEXT("normal")) OutItemData.Type = EItemDealType::Normal;
	
	InJsonObj->TryGetNumberField(TEXT("imgCount"), OutItemData.ImgCount);
	InJsonObj->TryGetNumberField(TEXT("startPrice"), OutItemData.StartPrice);
	InJsonObj->TryGetNumberField(TEXT("currentPrice"), OutItemData.CurrentPrice);

	bool haveGlb = false;
	InJsonObj->TryGetBoolField(TEXT("haveGLB"), haveGlb);
	OutItemData.HaveGlb = haveGlb;

	const TArray<TSharedPtr<FJsonValue>>* out; // 년 월 일 시간 분
	InJsonObj->TryGetArrayField(TEXT("endTime"), out);
	if(out->Num() >= 5) // 년 월 일 시간 분 을 모두 파싱할 수 있을지
	{
		OutItemData.EndTime = FDateTime((*out)[0]->AsNumber(),
										(*out)[1]->AsNumber(),
											(*out)[2]->AsNumber(),
											(*out)[3]->AsNumber(),
											(*out)[4]->AsNumber());
	}

	const TArray<TSharedPtr<FJsonValue>>* out2; // 년 월 일 시간 분 초
	InJsonObj->TryGetArrayField(TEXT("lastModifiedTime"), out2);
	if (out2->Num() >= 6) // 년 월 일 시간 분 초를  모두 파싱할 수 있을지
	{
		OutItemData.LastModifyTime = FDateTime((*out2)[0]->AsNumber(),
		                                       (*out2)[1]->AsNumber(),
												   (*out2)[2]->AsNumber(),
											       (*out2)[3]->AsNumber(),
											       (*out2)[4]->AsNumber(),
											        (*out2)[5]->AsNumber());
	}	
}

/**
 * Json 형태의 BidRecord JsonObject를 FBidRecord로 변환한다.
 * @param InJsonObj : Json 형태의 BidRecord String
 * @param OutBidRecord : 파싱한 정보를 담아갈 BidRecord
 */
void UItemDataHandler::_JsonToData(const TSharedPtr<FJsonObject>& InJsonObj, FBidRecord& OutBidRecord) const
{
	InJsonObj->TryGetNumberField(TEXT("id"), OutBidRecord.Order);
	InJsonObj->TryGetNumberField(TEXT("bidPrice"), OutBidRecord.BidPrice);
	InJsonObj->TryGetStringField(TEXT("bidUser"), OutBidRecord.BidUser);

	const TArray<TSharedPtr<FJsonValue>>* out; // 년 월 일 시간 분
	InJsonObj->TryGetArrayField(TEXT("bidTime"), out);
	if(out->Num() >= 6) // 년 월 일 시간 분 초를 모두 파싱할 수 있을지
	{
		OutBidRecord.BidTime = FDateTime((*out)[0]->AsNumber(),
		                                 (*out)[1]->AsNumber(),
											(*out)[2]->AsNumber(),
											(*out)[3]->AsNumber(),
											(*out)[4]->AsNumber(),
											 (*out)[5]->AsNumber());
	}
}