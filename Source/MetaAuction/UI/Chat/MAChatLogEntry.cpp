// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Chat/MAChatLogEntry.h"

FMAChatLogEntryData::FMAChatLogEntryData(const FText& InChatName, const FText& InChatLog)
{
	ChatName = InChatName;
	ChatLog = InChatLog;
}
