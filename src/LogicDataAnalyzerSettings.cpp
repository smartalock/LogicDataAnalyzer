#include "LogicDataAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


LogicDataAnalyzerSettings::LogicDataAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mBitRate( 1000 ),
	mInputChannelInterface(),
	mBitRateInterface()
{
	mInputChannelInterface.SetTitleAndTooltip( "Serial", "Standard LogicData Desk" );
	mInputChannelInterface.SetChannel( mInputChannel );

	mBitRateInterface.SetTitleAndTooltip( "Bit Rate (Bits/S)",  "Specify the bit rate in bits per second." );
	mBitRateInterface.SetMax( 6000000 );
	mBitRateInterface.SetMin( 1 );
	mBitRateInterface.SetInteger( mBitRate );

	AddInterface( &mInputChannelInterface );
	AddInterface( &mBitRateInterface );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "Serial", false );
}

LogicDataAnalyzerSettings::~LogicDataAnalyzerSettings()
{
}

bool LogicDataAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface.GetChannel();
	mBitRate = mBitRateInterface.GetInteger();

	ClearChannels();
	AddChannel( mInputChannel, "LogicData Desk", true );

	return true;
}

void LogicDataAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface.SetChannel( mInputChannel );
	mBitRateInterface.SetInteger( mBitRate );
}

void LogicDataAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;
	text_archive >> mBitRate;

	ClearChannels();
	AddChannel( mInputChannel, "LogicData Desk", true );

	UpdateInterfacesFromSettings();
}

const char* LogicDataAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;
	text_archive << mBitRate;

	return SetReturnString( text_archive.GetString() );
}
