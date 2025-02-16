#ifndef LOGICDATA_ANALYZER_H
#define LOGICDATA_ANALYZER_H

#include <Analyzer.h>
#include "LogicDataAnalyzerSettings.h"
#include "LogicDataAnalyzerResults.h"
#include "LogicDataSimulationDataGenerator.h"
#include <memory>

class ANALYZER_EXPORT LogicDataAnalyzer : public Analyzer2
{
public:
	LogicDataAnalyzer();
	virtual ~LogicDataAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	LogicDataAnalyzerSettings mSettings;
	std::unique_ptr<LogicDataAnalyzerResults> mResults;
	AnalyzerChannelData* mSerial;

	LogicDataSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //LOGICDATA_ANALYZER_H
