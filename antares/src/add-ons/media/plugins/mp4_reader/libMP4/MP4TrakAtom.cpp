/*
 * Copyright (c) 2005, David McPaul
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "MP4Parser.h"

TRAKAtom::TRAKAtom(BPositionIO *pStream, off_t pstreamOffset, uint32 patomType, uint64 patomSize) : AtomContainer(pStream, pstreamOffset, patomType, patomSize)
{
	theTKHDAtom = NULL;
	theMDHDAtom = NULL;
	framecount = 0;
	bytespersample = 0;
	timescale = 1;
}

TRAKAtom::~TRAKAtom()
{
}

void TRAKAtom::OnProcessMetaData()
{
}

char *TRAKAtom::OnGetAtomName()
{
	return "MPEG-4 Track Atom";
}

TKHDAtom	*TRAKAtom::getTKHDAtom()
{
	if (theTKHDAtom) {
		return theTKHDAtom;
	}
	
	theTKHDAtom = dynamic_cast<TKHDAtom *>(GetChildAtom(uint32('tkhd'),0));
	
	// Assert(theTKHDAtom != NULL,"Track has no Track Header");
	
	return theTKHDAtom;
}

MDHDAtom	*TRAKAtom::getMDHDAtom()
{
	if (theMDHDAtom) {
		return theMDHDAtom;
	}
	
	theMDHDAtom = dynamic_cast<MDHDAtom *>(GetChildAtom(uint32('mdhd'),0));
	
	// Assert(theMDHDAtom != NULL,"Track has no Media Header");
	
	return theMDHDAtom;
}

// Return duration of track
bigtime_t	TRAKAtom::Duration(uint32 TimeScale)
{
	if (IsAudio() || IsVideo()) {
		return getMDHDAtom()->getDuration();
	}
	
	return bigtime_t(getTKHDAtom()->getDuration() * 1000000.0) / TimeScale;
}

void TRAKAtom::OnChildProcessingComplete()
{
	timescale = getMDHDAtom()->getTimeScale();

	STTSAtom *aSTTSAtom = dynamic_cast<STTSAtom *>(GetChildAtom(uint32('stts'),0));

	if (aSTTSAtom) {
		framecount = aSTTSAtom->getSUMCounts();
		aSTTSAtom->setFrameRate(getFrameRate());
	}

}

// Is this a video track
bool TRAKAtom::IsVideo()
{
	// video tracks have a vmhd atom
	return (GetChildAtom(uint32('vmhd'),0) != NULL);
}

// Is this a audio track
bool TRAKAtom::IsAudio()
{
	// audio tracks have a smhd atom
	return (GetChildAtom(uint32('smhd'),0) != NULL);
}

// frames per us
float	TRAKAtom::getFrameRate() 
{
	if (IsAudio()) {
		AtomBase *aAtomBase = GetChildAtom(uint32('stsd'),0);
		if (aAtomBase) {
			STSDAtom *aSTSDAtom = dynamic_cast<STSDAtom *>(aAtomBase);

			AudioDescription aAudioDescription = aSTSDAtom->getAsAudio();
			return (aAudioDescription.theAudioSampleEntry.SampleRate / 65536.0) / aAudioDescription.FrameSize;
		}
	} else if (IsVideo()) {
		return (framecount * 1000000.0) / Duration();
	}
	
	return 0.0;
}

float	TRAKAtom::getSampleRate() 
{
	// Only valid for Audio
	if (IsAudio()) {
		AtomBase *aAtomBase = GetChildAtom(uint32('stsd'),0);
		if (aAtomBase) {
			STSDAtom *aSTSDAtom = dynamic_cast<STSDAtom *>(aAtomBase);

			AudioDescription aAudioDescription = aSTSDAtom->getAsAudio();
			return aAudioDescription.theAudioSampleEntry.SampleRate / 65536;
		}
	}
	
	return 0.0;
}

bigtime_t	TRAKAtom::getTimeForFrame(uint32 pFrame)
{
	return bigtime_t((pFrame * 1000000.0) / getFrameRate());
}

uint32	TRAKAtom::getSampleForTime(bigtime_t pTime)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stts'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STTSAtom *>(aAtomBase))->getSampleForTime(pTime);
	}
	
	return 0;
}

uint32	TRAKAtom::getSampleForFrame(uint32 pFrame)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stts'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STTSAtom *>(aAtomBase))->getSampleForTime(getTimeForFrame(pFrame));
	}
	
	return 0;
}

uint32	TRAKAtom::getChunkForSample(uint32 pSample, uint32 *pOffsetInChunk)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stsc'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STSCAtom *>(aAtomBase))->getChunkForSample(pSample, pOffsetInChunk);
	}
	
	return 0;
}

uint32	TRAKAtom::getNoSamplesInChunk(uint32 pChunkID)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stsc'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STSCAtom *>(aAtomBase))->getNoSamplesInChunk(pChunkID);
	}
	
	return 0;
}

uint32	TRAKAtom::getSizeForSample(uint32 pSample)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stsz'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STSZAtom *>(aAtomBase))->getSizeForSample(pSample);
	}
	
	return 0;
}

uint64	TRAKAtom::getOffsetForChunk(uint32 pChunkID)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stco'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STCOAtom *>(aAtomBase))->getOffsetForChunk(pChunkID);
	}
	
	return 0;
}

uint32	TRAKAtom::getFirstSampleInChunk(uint32 pChunkID)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stsc'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STSCAtom *>(aAtomBase))->getFirstSampleInChunk(pChunkID);
	}
	
	return 0;
}


bool	TRAKAtom::IsSyncSample(uint32 pSampleNo)
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stss'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STSSAtom *>(aAtomBase))->IsSyncSample(pSampleNo);
	}
	
	return false;
}

bool	TRAKAtom::IsSingleSampleSize()
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stsz'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STSZAtom *>(aAtomBase))->IsSingleSampleSize();
	}
	
	return false;
}

bool	TRAKAtom::IsActive()
{
	return getTKHDAtom()->IsActive();
}

uint32	TRAKAtom::getBytesPerSample()
{
AtomBase *aAtomBase;

	if (bytespersample > 0) {
		return bytespersample;
	}
	
	// calculate bytes per sample and cache it
	
	if (IsAudio()) {
		// only used by Audio
		aAtomBase = GetChildAtom(uint32('stsd'),0);
		if (aAtomBase) {
			STSDAtom *aSTSDAtom = dynamic_cast<STSDAtom *>(aAtomBase);

			AudioDescription aAudioDescription = aSTSDAtom->getAsAudio();

			bytespersample = aAudioDescription.theAudioSampleEntry.SampleSize / 8;
		}
	}
	
	return bytespersample;	
}

uint32	TRAKAtom::getTotalChunks()
{
	AtomBase *aAtomBase = GetChildAtom(uint32('stco'),0);
	
	if (aAtomBase) {
		return (dynamic_cast<STCOAtom *>(aAtomBase))->getTotalChunks();
	}
	
	return 0;
}
