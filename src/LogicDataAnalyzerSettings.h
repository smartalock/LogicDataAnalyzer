#ifndef LOGICDATA_ANALYZER_SETTINGS
#define LOGICDATA_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class LogicDataAnalyzerSettings : public AnalyzerSettings
{
public:
	LogicDataAnalyzerSettings();
	virtual ~LogicDataAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mBitRate;

protected:
	AnalyzerSettingInterfaceChannel	mInputChannelInterface;
	AnalyzerSettingInterfaceInteger	mBitRateInterface;
};

#endif //LOGICDATA_ANALYZER_SETTINGS
