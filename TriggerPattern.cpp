#include "TriggerPattern.h"

using namespace plogue::biduleSDK;
using namespace acme;

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

uint32_t xxxor128(void)
{
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

TriggerPattern::TriggerPattern(BiduleHost *host) : BidulePlugin(host)
{
    //fill in plugin capabilities as a mask
    //  _caps = CAP_SYNCMASTER | CAP_SYNCSLAVE
    _caps = 0;

    _numAudioIns = 3;
    _numAudioOuts = 16;
    _numMIDIIns = 0;
    _numMIDIOuts = 0;
    _numFreqIns = 0;
    _numFreqOuts = 0;
    _numMagIns = 0;
    _numMagOuts = 0;
    
    _numParams = 5;
    _numUIColumns = 1;
    
    _index = 0;
    _lower = 0;
    _length = 16;
    _pattern = new int[256];
    _minOutBound = 0;
    _maxOutBound = 16;
//    _patternCache = "";
}


TriggerPattern::~TriggerPattern() {}

bool TriggerPattern::init()
{
    cout << "TriggerPattern init" << endl;
    getParameterValue(4, _patternCache);
    
    if (_patternCache.length() > 0) {
        cout << "running presetToPattern" << endl;
        presetToPattern(_patternCache);
    } else {
        cout << "initialising _pattern array" << endl;
        for (int i = 0; i < 256; i++)
            _pattern[i] = i % _numAudioOuts;
    }

    return true;
}

void TriggerPattern::getAudioInNames(std::vector<std::string> &vec)
{
    vec.push_back("Trigger");
    vec.push_back("New Pattern");
    vec.push_back("Reset");
}

void TriggerPattern::getAudioOutNames(std::vector<std::string> &vec)
{
    vec.push_back("T0");
    vec.push_back("T1");
    vec.push_back("T2");
    vec.push_back("T3");
    vec.push_back("T4");
    vec.push_back("T5");
    vec.push_back("T6");
    vec.push_back("T7");
    vec.push_back("T8");
    vec.push_back("T9");
    vec.push_back("TA");
    vec.push_back("TB");
    vec.push_back("TC");
    vec.push_back("TD");
    vec.push_back("TE");
    vec.push_back("TF");
}

void TriggerPattern::getParametersInfos(ParameterInfo *pinfos)
{
    pinfos[0].id = 0;
    strcpy(pinfos[0].name, "Start");
    pinfos[0].type = DOUBLEPARAM;
    pinfos[0].ctrlType = GUICTRL_SLIDER;
    pinfos[0].linkable = 1;
    pinfos[0].saveable = 1;
    pinfos[0].paramInfo.pd.defaultValue = 0;
    pinfos[0].paramInfo.pd.minValue = 0;
    pinfos[0].paramInfo.pd.maxValue = 256;
    pinfos[0].paramInfo.pd.precision = 0;

    pinfos[1].id = 1;
    strcpy(pinfos[1].name, "Length");
    pinfos[1].type = DOUBLEPARAM;
    pinfos[1].ctrlType = GUICTRL_SLIDER;
    pinfos[1].linkable = 1;
    pinfos[1].saveable = 1;
    pinfos[1].paramInfo.pd.defaultValue = 16;
    pinfos[1].paramInfo.pd.minValue = 0;
    pinfos[1].paramInfo.pd.maxValue = 256;
    pinfos[1].paramInfo.pd.precision = 0;

    pinfos[2].id = 2;
    strcpy(pinfos[2].name, "Min Out");
    pinfos[2].type = DOUBLEPARAM;
    pinfos[2].ctrlType = GUICTRL_SLIDER;
    pinfos[2].linkable = 1;
    pinfos[2].saveable = 1;
    pinfos[2].paramInfo.pd.defaultValue = 0;
    pinfos[2].paramInfo.pd.minValue = 0;
    pinfos[2].paramInfo.pd.maxValue = 16;
    pinfos[2].paramInfo.pd.precision = 0;

    pinfos[3].id = 3;
    strcpy(pinfos[3].name, "Max Out");
    pinfos[3].type = DOUBLEPARAM;
    pinfos[3].ctrlType = GUICTRL_SLIDER;
    pinfos[3].linkable = 1;
    pinfos[3].saveable = 1;
    pinfos[3].paramInfo.pd.defaultValue = 0;
    pinfos[3].paramInfo.pd.minValue = 0;
    pinfos[3].paramInfo.pd.maxValue = 16;
    pinfos[3].paramInfo.pd.precision = 0;
    
    pinfos[4].id = 4;
    strcpy(pinfos[4].name, "Current pattern data");
    pinfos[4].type = STRINGPARAM;
    pinfos[4].ctrlType = GUICTRL_NOGUI;
    pinfos[4].linkable = 0;
    pinfos[4].saveable = 1;
//    pinfos[4].paramInfo.ps.defaultValue[0] = 1;
    
}

void TriggerPattern::getParameterChoices(long id, std::vector<std::string> &vec) {}

void TriggerPattern::parameterUpdate(long id)
{
    cout << "ID " << id << " CALLED parameterUpdate" << endl;
    
    if (id == 0)
//        cout << "ID " << id << " CALLED parameterUpdate" << endl;
        getParameterValue(0, _lower);
    if (id == 1)
//        cout << "ID " << id << " CALLED parameterUpdate" << endl;
        getParameterValue(1, _length);
    if (id == 2)
//        cout << "ID " << id << " CALLED parameterUpdate" << endl;
        getParameterValue(2, _minOutBound);
    if (id == 3)
        getParameterValue(3, _maxOutBound);
    if (id == 4)
//        cout << "pattern data updated!" << endl;
//        cout << "textarea pinged/updated!" << endl;
        getParameterValue(4, _patternCache);
        presetToPattern(_patternCache);
}


void TriggerPattern::presetToPattern(string &_patternCache) {
    vector<string> out;
    string token;
    std::istringstream stream(_patternCache);
    
    while (std::getline(stream, token, ';')) {
        out.push_back(token);
    }
    
    for (int i=0; i < out.size(); i++) {
      _pattern[i] = stoi(out.at(i));
//      cout << "numbersOld index " << i << ": " << _pattern[i] << endl;
    }
}

void TriggerPattern::process(Sample **sampleIn, Sample **sampleOut, MIDIEvents *midiIn, MIDIEvents *midiOut, Frequency ***freqIn, Frequency ***freqOut,
                             Magnitude ***magIn, Magnitude ***magOut, SyncInfo *syncIn, SyncInfo *syncOut)
{

    Sample *in0 = sampleIn[0];
    Sample *in1 = sampleIn[1];
    Sample *in2 = sampleIn[2];

    long sampleFrames = _dspInfo.bufferSize;

    for (int sample = 0; sample < sampleFrames; sample++)
    {
        Sample trigger = (*in0++);
        Sample newPattern = (*in1++);
        Sample reset = (*in2++);
        
        if (reset == 1.f)
            _index = 0;
        
        if (trigger == 1.f) {
            _index = (_index + 1);
            if (_index >= _lower + _length)
                _index = _lower;
            if(_index < _lower)
                _index = _lower;
        }
        
        if (newPattern == 1.f) {
            string str = "";
            
            for (int i = 0; i < 256; i++) {
                _pattern[i] = abs((int)xxxor128());
                str += to_string(_pattern[i]) + ";";
            }
            
            updateParameter(4, str);

        }

        int t = _pattern[_index % 256];

        int minOut = _minOutBound < _maxOutBound ? _minOutBound : _maxOutBound;
        int maxOut = _maxOutBound > _minOutBound ? _maxOutBound : _minOutBound;
        int w = maxOut - minOut;
        if (w == 0)
            t = minOut;
        else
            t = minOut + (t % w);
        for (int i = minOut; i < maxOut; i++)
        {
            sampleOut[i][sample] = i == t ? trigger : 0;
        }
    }
}
