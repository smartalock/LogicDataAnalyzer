#ifndef LOGICDATA_SIMULATION_DATA_GENERATOR
#define LOGICDATA_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class LogicDataAnalyzerSettings;

class LogicDataSimulationDataGenerator
{
public:
	LogicDataSimulationDataGenerator();
	~LogicDataSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, LogicDataAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	LogicDataAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;
	U32 mSamplesPerBit;

protected:
	void CreateSerialWord();
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //LOGICDATA_SIMULATION_DATA_GENERATOR