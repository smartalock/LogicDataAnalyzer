#include "LogicDataSimulationDataGenerator.h"
#include "LogicDataAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

#define TEST_DATA_COUNT (3)
static uint32_t test_data[TEST_DATA_COUNT] = {
    0x40681400,
    0x4060043c,
    0x40649dfb,
};


LogicDataSimulationDataGenerator::LogicDataSimulationDataGenerator() :
	mStringIndex( 0 )
{
}

LogicDataSimulationDataGenerator::~LogicDataSimulationDataGenerator()
{
}

void LogicDataSimulationDataGenerator::Initialize( U32 simulation_sample_rate, LogicDataAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSerialSimulationData.SetChannel( mSettings->mInputChannel );
	mSerialSimulationData.SetSampleRate( simulation_sample_rate );
	mSerialSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 LogicDataSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mSerialSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreateSerialWord();
	}

	*simulation_channel = &mSerialSimulationData;
	return 1;
}

void LogicDataSimulationDataGenerator::CreateSerialWord()
{
	U32 samples_per_bit = mSimulationSampleRateHz / mSettings->mBitRate;

	if (mStringIndex == 0) {
		mSerialSimulationData.TransitionIfNeeded( BIT_HIGH );
		mSerialSimulationData.Advance( samples_per_bit * 300 );
	}

	uint32_t word = test_data[mStringIndex];
	mStringIndex++;
	if (mStringIndex >= TEST_DATA_COUNT) {
		mStringIndex = 0;
	}


	// Write 50ms SYNC
	mSerialSimulationData.TransitionIfNeeded( BIT_LOW );
	mSerialSimulationData.Advance( samples_per_bit * 50 );
	for( U32 i=0; i<32; i++ ) {
		if (word & 0x80000000) {
			mSerialSimulationData.TransitionIfNeeded( BIT_LOW );
		} else {
			mSerialSimulationData.TransitionIfNeeded( BIT_HIGH );
		}
		mSerialSimulationData.Advance( samples_per_bit );
		word = word << 1;
	}

}
