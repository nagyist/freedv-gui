//=========================================================================
// Name:            FreeDVTransmitStep.cpp
// Purpose:         Describes a modulation step in the audio pipeline.
//
// Authors:         Mooneer Salem
// License:
//
//  All rights reserved.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.1,
//  as published by the Free Software Foundation.  This program is
//  distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//=========================================================================

#include <cassert>
#include "FreeDVTransmitStep.h"
#include "freedv_api.h"

FreeDVTransmitStep::FreeDVTransmitStep(FreeDVInterface& iface, std::function<float()> getFreqOffsetFn)
    : interface_(iface)
    , getFreqOffsetFn_(getFreqOffsetFn)
{
    // empty
}

FreeDVTransmitStep::~FreeDVTransmitStep()
{
    // empty
}

int FreeDVTransmitStep::getInputSampleRate() const
{
    return interface_.getTxSpeechSampleRate();
}

int FreeDVTransmitStep::getOutputSampleRate() const
{
    return interface_.getTxModemSampleRate();
}

std::shared_ptr<short> FreeDVTransmitStep::execute(std::shared_ptr<short> inputSamples, int numInputSamples, int* numOutputSamples)
{
    short* outputSamples = nullptr;

    if (numInputSamples > 0)
    {
        auto mode = interface_.getTxMode();
    
        *numOutputSamples = interface_.getTxNNomModemSamples();
        outputSamples = new short[*numOutputSamples];
        assert(outputSamples != nullptr);
    
        if (mode == FREEDV_MODE_800XA || mode == FREEDV_MODE_2400B) 
        {
            /* 800XA doesn't support complex output just yet */
            interface_.transmit(outputSamples, inputSamples.get());
        }
        else 
        {
            interface_.complexTransmit(outputSamples, inputSamples.get(), getFreqOffsetFn_(), *numOutputSamples);
        }
    }
    else
    {
        *numOutputSamples = 0;
    }

    return std::shared_ptr<short>(outputSamples, std::default_delete<short[]>());
}