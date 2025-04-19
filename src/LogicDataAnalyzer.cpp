#include "LogicDataAnalyzer.h"
#include "LogicDataAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

LogicDataAnalyzer::LogicDataAnalyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
}

LogicDataAnalyzer::~LogicDataAnalyzer()
{
	KillThread();
}

void LogicDataAnalyzer::SetupResults()
{
	// SetupResults is called each time the analyzer is run. Because the same instance can be used for multiple runs, we need to clear the results each time.
	mResults.reset(new LogicDataAnalyzerResults( this, &mSettings ));
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings.mInputChannel );
}

void LogicDataAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();

	mSerial = GetAnalyzerChannelData( mSettings.mInputChannel );

	U32 samples_per_bit = mSampleRateHz / mSettings.mBitRate;

	for( ; ; )
	{
		if (mSerial->GetBitState() == BIT_HIGH) {
			mSerial->AdvanceToNextEdge();
		}

		U64 sync_start = mSerial->GetSampleNumber();

		mSerial->AdvanceToNextEdge();

		U64 sync_finish = mSerial->GetSampleNumber();

		if (((sync_finish - sync_start) / samples_per_bit) > 32) {
			// We have found a valid sync and are now positioned at the start of the data
			mResults->AddMarker( sync_start, AnalyzerResults::Square, mSettings.mInputChannel );

			Frame syncFrame;
			syncFrame.mData1 = 0;
			syncFrame.mType = 1;
			syncFrame.mFlags = 0;
			syncFrame.mStartingSampleInclusive = sync_start;
			syncFrame.mEndingSampleInclusive = mSerial->GetSampleNumber();

			mResults->AddFrame( syncFrame );
			mResults->CommitResults();
			ReportProgress( syncFrame.mEndingSampleInclusive );


			mSerial->Advance(0.5 * samples_per_bit);
			U32 word = 0;
			U8 ones = 0;

			for( U32 i=0; i < 32; i++ ) {
				word = word >> 1;
				if (mSerial->GetBitState() == BIT_HIGH) {
					mResults->AddMarker( mSerial->GetSampleNumber(), AnalyzerResults::Zero, mSettings.mInputChannel );
				} else {
					ones++;
					mResults->AddMarker( mSerial->GetSampleNumber(), AnalyzerResults::One, mSettings.mInputChannel );
					word |= 0x80000000;
				}

				if (i == 31) {
					// Check parity
					if ((ones % 2) == 0) {
						// Parity OK
					} else {
						// Parity Error
						mResults->AddMarker( mSerial->GetSampleNumber(), AnalyzerResults::ErrorX, mSettings.mInputChannel );
					}
				}

				if (i < 31) {
					mSerial->Advance(samples_per_bit);
				}
			}

			mSerial->Advance(0.5 * samples_per_bit);

			// Add frame
			//we have a byte to save. 
			Frame dataFrame;
			dataFrame.mData1 = word;
			dataFrame.mType = 2;
			dataFrame.mFlags = 0;
			dataFrame.mStartingSampleInclusive = sync_finish;
			dataFrame.mEndingSampleInclusive = mSerial->GetSampleNumber();

			if (word != 0) {
				mResults->AddFrame( dataFrame );
				mResults->CommitResults();
			}
			ReportProgress( dataFrame.mEndingSampleInclusive );

		} else {
			// Bad sync
			mResults->AddMarker( sync_start, AnalyzerResults::ErrorSquare, mSettings.mInputChannel );
		}
	}
}

bool LogicDataAnalyzer::NeedsRerun()
{
	return false;
}

U32 LogicDataAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 LogicDataAnalyzer::GetMinimumSampleRateHz()
{
	return mSettings.mBitRate * 4;
}

const char* LogicDataAnalyzer::GetAnalyzerName() const
{
	return "LogicData Desk";
}

const char* GetAnalyzerName()
{
	return "LogicData Desk";
}

Analyzer* CreateAnalyzer()
{
	return new LogicDataAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}