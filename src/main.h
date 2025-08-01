//==========================================================================
// Name:            main.h
//
// Purpose:         Declares simple wxWidgets application with GUI.
// Created:         Apr. 9, 2012
// Authors:         David Rowe, David Witten
//
// License:
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General License version 2.1,
//  as published by the Free Software Foundation.  This program is
//  distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General License
//  along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//==========================================================================
#ifndef __FDMDV2_MAIN__
#define __FDMDV2_MAIN__

#include "config.h"
#include <wx/wx.h>

#include <wx/tglbtn.h>
#include <wx/app.h>
#include "wx/rawbmp.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/config.h"
#include <wx/fileconf.h>
#include "wx/graphics.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"
#include "wx/quantize.h"
#include "wx/scopedptr.h"
#include "wx/stopwatch.h"
#include "wx/versioninfo.h"
#include <wx/sound.h>
#include <wx/url.h>
#include <wx/sstream.h>
#include <wx/listbox.h>
#include <wx/textdlg.h>
#include <wx/regex.h>
#include <wx/socket.h>

#include <samplerate.h>

#include <stdint.h>
#include <speex/speex_preprocess.h>
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <cpuid.h>
#endif
#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#include <dlfcn.h>
#endif

#ifdef _MSC_VER
// used for AVX checking
#include <intrin.h>
#endif

#include "codec2.h"
#include "codec2_fifo.h"
#include "modem_stats.h"

#include "topFrame.h"
#include "gui/controls/plot.h"
#include "gui/controls/plot_scalar.h"
#include "gui/controls/plot_scatter.h"
#include "gui/controls/plot_waterfall.h"
#include "gui/controls/plot_spectrum.h"
#include "sndfile.h"
#include "sox_biquad.h"
#include "comp_prim.h"
#include "rig_control/HamlibRigController.h"
#include "rig_control/SerialPortOutRigController.h"
#include "rig_control/SerialPortInRigController.h"
#include "reporting/IReporter.h"
#include "reporting/FreeDVReporter.h"
#include "freedv_interface.h"
#include "audio/AudioEngineFactory.h"
#include "audio/IAudioDevice.h"
#include "config/FreeDVConfiguration.h"
#include "pipeline/paCallbackData.h"
#include "pipeline/LinkStep.h"

#define _USE_TIMER              1
#define _USE_ONIDLE             1
#define _DUMMY_DATA             1
//#define _AUDIO_PASSTHROUGH    1
#define _REFRESH_TIMER_PERIOD   (DT*1000)

//#define _USE_ABOUT_DIALOG       1

enum {
        ID_START = wxID_HIGHEST,
        ID_TIMER_WATERFALL,
        ID_TIMER_SPECTRUM,
        ID_TIMER_SCATTER,
        ID_TIMER_SCALAR,
        ID_TIMER_PSKREPORTER,
        ID_TIMER_UPD_FREQ,
     };

#define EXCHANGE_DATA_IN    0
#define EXCHANGE_DATA_OUT   1

extern int                 g_nSoundCards;

// Voice Keyer Constants

#define VK_SYNC_WAIT_TIME 5.0

// Voice Keyer States

#define VK_IDLE      0
#define VK_TX        1
#define VK_RX        2
#define VK_SYNC_WAIT 3

// Voice Keyer Events

#define VK_START         0
#define VK_SPACE_BAR     1
#define VK_PLAY_FINISHED 2
#define VK_DT            3
#define VK_SYNC          4

// "Detect Sync" state machine states and constants

#define DS_IDLE           0
#define DS_SYNC_WAIT      1
#define DS_UNSYNC_WAIT    2
#define DS_SYNC_WAIT_TIME 5.0

class MainFrame;
class FilterDlg;
class FreeDVReporterDialog;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
// Class MainApp
//
// @class $(Name)
// @author $(User)
// @date $(Date)
// @file $(CurrentFileName).$(CurrentFileExt)
// @brief
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
class MainApp : public wxApp
{
    public:
        virtual bool        OnInit();
        virtual void        OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool        OnCmdLineParsed(wxCmdLineParser& parser);
        virtual int         OnExit();


        bool                    CanAccessSerialPort(std::string portName);
        
        FreeDVConfiguration appConfiguration;
        wxString customConfigFileName;
        
        // PTT -----------------------------------    
        unsigned int        m_intHamlibRig;
        std::shared_ptr<IRigFrequencyController> rigFrequencyController;
        std::shared_ptr<IRigPttController> rigPttController;

        // PTT Input
        std::shared_ptr<SerialPortInRigController> m_pttInSerialPort;

        wxRect              m_rTopWindow;

        // To support viewing FreeDV Reporter data outside of a session, we need to have
        // a running connection and know when to appropriately kill it. A shared_ptr
        // allows us to do so.
        std::shared_ptr<FreeDVReporter> m_sharedReporterObject;
        
        std::vector<std::shared_ptr<IReporter> > m_reporters;
        
        bool                loadConfig();
        bool                saveConfig();

        // misc

        bool       m_testFrames;
        bool       m_channel_noise;
        float      m_channel_snr_dB;

        int        FilterEvent(wxEvent& event);
        MainFrame *frame;

        // 700 options
        bool       m_FreeDV700Combine;

        // carrier attenuation

        bool       m_attn_carrier_en;
        int        m_attn_carrier;

        // tone interferer simulation

        bool       m_tone;
        int        m_tone_freq_hz;
        int        m_tone_amplitude;

        // debugging 700D audio break up

        bool       m_txRxThreadHighPriority;

        int        m_prevMode;
        
        std::shared_ptr<LinkStep> linkStep;

        wxLocale m_locale;

        int m_reportCounter;
    protected:
    private:
        void UnitTest_();
};

// declare global static function wxGetApp()
DECLARE_APP(MainApp)

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
// panel with custom loop checkbox for play file dialog
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
class MyExtraPlayFilePanel : public wxPanel
{
public:
    MyExtraPlayFilePanel(wxWindow *parent);
    void setLoopPlayFileToMicIn(bool checked) { m_cb->SetValue(checked); }
    bool getLoopPlayFileToMicIn(void) { return m_cb->GetValue(); }
private:
    wxCheckBox *m_cb;
};

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
// panel with custom Seconds-to-record control for record file dialog
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
class MyExtraRecFilePanel : public wxPanel
{
public:
    MyExtraRecFilePanel(wxWindow *parent);
    ~MyExtraRecFilePanel()
    {
        wxLogDebug("Destructor\n");
    }
    void setSecondsToRecord(wxString value) { m_secondsToRecord->SetValue(value); }
    wxString getSecondsToRecord(void)
    {
        wxLogDebug("getSecondsToRecord: %s\n",m_secondsToRecord->GetValue());
        return m_secondsToRecord->GetValue();
    }
private:
    wxTextCtrl *m_secondsToRecord;
};

class TxRxThread;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
// Class MainFrame
//
// @class $(Name)
// @author $(User)
// @date $(Date)
// @file $(CurrentFileName).$(CurrentFileExt)
// @brief
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=
class MainFrame : public TopFrame
{
    public:
        MainFrame(wxWindow *parent);
        virtual ~MainFrame();

        FilterDlg*              m_filterDialog;
        FreeDVReporterDialog*   m_reporterDialog;
        PlotSpectrum*           m_panelSpectrum;
        PlotWaterfall*          m_panelWaterfall;
        PlotScatter*            m_panelScatter;
        PlotScalar*             m_panelTimeOffset;
        PlotScalar*             m_panelFreqOffset;
        PlotScalar*             m_panelSpeechIn;
        PlotScalar*             m_panelSpeechOut;
        PlotScalar*             m_panelDemodIn;
        PlotScalar*             m_panelTestFrameErrors;
        PlotScalar*             m_panelTestFrameErrorsHist;
        wxComboBox*             m_cbxNumSpectrumAveraging;

        bool                    m_RxRunning;

        TxRxThread*             m_txThread;
        TxRxThread*             m_rxThread;
        
        bool                    OpenHamlibRig();
#if defined(WIN32)
        void                    OpenOmniRig();
#endif // defined(WIN32)
        void                    OpenSerialPort(void);
        void                    OpenPTTInPort(void);
        void                    ClosePTTInPort(void);

        bool                    m_modal;

#ifdef _USE_TIMER
        wxTimer                 m_plotTimer;
        
        // Not sure why we have the option to disable timers. TBD?
        wxTimer                 m_pskReporterTimer;
        wxTimer                 m_updFreqStatusTimer; //[UP]
#endif

    void destroy_fifos(void);

    void togglePTT(void);

    bool                    m_schedule_restore;

    // Voice Keyer state machine

    int                     vk_state;
    void VoiceKeyerProcessEvent(int vk_event);

    // Detect Sync state machine

    int                     ds_state;
    float                   ds_rx_time;
    void DetectSyncProcessEvent(void);

        void StopPlayFileToMicIn(void);
        void StopPlaybackFileFromRadio();
        void StopRecFileFromRadio();
        
        bool isReceiveOnly();
        
    protected:

        void setsnrBeta(bool snrSlow);

        // protected event handlers
        virtual void topFrame_OnSize( wxSizeEvent& event ) override;
        virtual void topFrame_OnClose( wxCloseEvent& event ) override;
        virtual void OnCloseFrame(wxCloseEvent& event);
        void OnExitClick(wxCommandEvent& event);
        
        void OnAveragingChange(wxCommandEvent& event);

        void startTxStream();
        void startRxStream();
        void stopTxStream();
        void stopRxStream();
        void abortTxStream();
        void abortRxStream();

        void OnTop(wxCommandEvent& event) override;
        void OnExit( wxCommandEvent& event ) override;

        void OnToolsEasySetup( wxCommandEvent& event ) override;
        void OnToolsEasySetupUI( wxUpdateUIEvent& event ) override;
        void OnToolsFreeDVReporter( wxCommandEvent& event ) override;
        void OnToolsFreeDVReporterUI( wxUpdateUIEvent& event ) override;
        void OnToolsAudio( wxCommandEvent& event ) override;
        void OnToolsAudioUI( wxUpdateUIEvent& event ) override;
        void OnToolsComCfg( wxCommandEvent& event ) override;
        void OnToolsComCfgUI( wxUpdateUIEvent& event ) override;
        void OnToolsFilter( wxCommandEvent& event ) override;
        void OnToolsOptions(wxCommandEvent& event) override;
        void OnToolsOptionsUI(wxUpdateUIEvent& event) override;

        void OnRecFileFromRadio( wxCommandEvent& event ) override;
        void OnPlayFileFromRadio( wxCommandEvent& event ) override;
        
        void OnCenterRx(wxCommandEvent& event) override;

        void OnHelpCheckUpdates( wxCommandEvent& event ) override;
        void OnHelpCheckUpdatesUI( wxUpdateUIEvent& event ) override;
        void OnHelpAbout( wxCommandEvent& event ) override;
        void OnHelpManual( wxCommandEvent& event ) override;
        void OnCmdSliderScroll( wxScrollEvent& event ) override;
        void OnCheckSQClick( wxCommandEvent& event ) override;
        void OnCheckSNRClick( wxCommandEvent& event ) override;

        // Toggle Buttons
        void OnTogBtnSplitClick(wxCommandEvent& event);
        void OnTogBtnAnalogClick(wxCommandEvent& event) override;
        void OnTogBtnPTT( wxCommandEvent& event ) override;
        void OnTogBtnPTTRightClick( wxContextMenuEvent& event ) override;
        void OnTogBtnVoiceKeyerClick (wxCommandEvent& event) override;
        void OnTogBtnVoiceKeyerRightClick( wxContextMenuEvent& event ) override;
        
        void OnHelp( wxCommandEvent& event ) override;

        void OnTogBtnOnOff( wxCommandEvent& event ) override;
        void OnTogBtnRecord( wxCommandEvent& event ) override;

        void OnCallSignReset( wxCommandEvent& event ) override;
        void OnBerReset( wxCommandEvent& event ) override;
        void OnReSync( wxCommandEvent& event ) override;

        //System Events
        void OnPaint(wxPaintEvent& event);
        void OnSize( wxSizeEvent& event );
        void OnUpdateUI( wxUpdateUIEvent& event );
        void OnDeleteConfig(wxCommandEvent&);
        void OnDeleteConfigUI( wxUpdateUIEvent& event );
#ifdef _USE_TIMER
        void OnTimer(wxTimerEvent &evt);
#endif
#ifdef _USE_ONIDLE
        void OnIdle(wxIdleEvent &evt);
#endif

        int VoiceKeyerStartTx(void);

        void OnChangeTxMode( wxCommandEvent& event ) override;
        
        void OnChangeTxLevel( wxScrollEvent& event ) override;
        
        void OnChangeMicSpkrLevel( wxScrollEvent& event ) override;
        
        void OnChangeReportFrequency( wxCommandEvent& event ) override;
        void OnChangeReportFrequencyVerify( wxCommandEvent& event ) override;
        
        void OnReportFrequencySetFocus(wxFocusEvent& event) override;
        void OnReportFrequencyKillFocus(wxFocusEvent& event) override;

        void OnSystemColorChanged(wxSysColourChangedEvent& event) override;
        
        void OnNotebookPageChanging(wxAuiNotebookEvent& event) override;
        
        void OnChooseAlternateVoiceKeyerFile( wxCommandEvent& event );
        void OnRecordNewVoiceKeyerFile( wxCommandEvent& event );
        
        void OnSetMonitorVKAudio( wxCommandEvent& event );
        void OnSetMonitorTxAudio( wxCommandEvent& event );
        
        void OnSetMonitorVKAudioVol( wxCommandEvent& event );
        void OnSetMonitorTxAudioVol( wxCommandEvent& event );
        
    private:
        friend class MainApp; // needed for unit tests
        
        std::shared_ptr<IAudioDevice> rxInSoundDevice;
        std::shared_ptr<IAudioDevice> rxOutSoundDevice;
        std::shared_ptr<IAudioDevice> txInSoundDevice;
        std::shared_ptr<IAudioDevice> txOutSoundDevice;
        
        unsigned int         m_timeSinceSyncLoss;
        bool        m_useMemory;
        wxTextCtrl* m_tc;
        int         m_zoom;
        float       m_snrBeta;

        // Callsign/text messaging
        char        m_callsign[MAX_CALLSIGN];
        char       *m_pcallsign;

        // Events
        void        processTxtEvent(char event[]);
        class OptionsDlg *optionsDlg;

        // level Gauge
        float       m_maxLevel;

        // flags to indicate when new EQ filters need to be designed

        bool        m_newMicInFilter;
        bool        m_newSpkOutFilter;

        std::shared_ptr<void>       designAnEQFilter(const char filterType[], float freqHz, float gaindB, float Q = 0.0, int sampleRate = 8000);
        void        designEQFilters(paCallBackData *cb, int rxSampleRate, int txSampleRate);
        void        deleteEQFilters(paCallBackData *cb);

        // Voice Keyer States

        int        vk_rx_pause;
        int        vk_repeats, vk_repeat_counter;
        float      vk_rx_time;
        float      vk_rx_sync_time;
        bool suppressFreqModeUpdates_;
        bool firstFreqUpdateOnConnect_;
        
        std::string vkFileName_;
        
        wxMenu* voiceKeyerPopupMenu_;
        wxMenu* pttPopupMenu_;
        wxMenuItem* adjustMonitorPttVolMenuItem_;
        wxMenuItem* adjustMonitorVKVolMenuItem_;
        wxMenuItem* chooseVKFileMenuItem_;
        wxMenuItem* recordNewVoiceKeyerFileMenuItem_;

        bool terminating_; // used for terminating FreeDV
        bool realigned_; // used to inhibit resize hack once already done
        
        int         getSoundCardIDFromName(wxString& name, bool input);
        bool        validateSoundCardSetup();
        
        void loadConfiguration_();
        void resetStats_();

        HamlibRigController::Mode getCurrentMode_();
        
        void performFreeDVOn_();
        void performFreeDVOff_();
        
        void executeOnUiThreadAndWait_(std::function<void()> fn);
        
        void updateReportingFreqList_();
        
        void initializeFreeDVReporter_();
        
        void onFrequencyModeChange_(IRigFrequencyController*, uint64_t freq, IRigFrequencyController::Mode mode);
        void onRadioConnected_(IRigController* ptr);
        void onRadioDisconnected_(IRigController* ptr);
};

void resample_for_plot(struct FIFO *plotFifo, short buf[], short* dec_samples, int length, int fs);

int resample(SRC_STATE *src,
             short      output_short[],
             short      input_short[],
             int        output_sample_rate,
             int        input_sample_rate,
             int        length_output_short, // maximum output array length in samples
             int        length_input_short
             );
void txRxProcessing();

// FreeDv API calls this when there is a test frame that needs a-plottin'

void my_freedv_put_error_pattern(void *state, short error_pattern[], int sz_error_pattern);

// FreeDv API calls these puppies when it needs/receives a text char

char my_get_next_tx_char(void *callback_state);
void my_put_next_rx_char(void *callback_state, char c);

// helper complex freq shift function

void freq_shift_coh(COMP rx_fdm_fcorr[], COMP rx_fdm[], float foff, float Fs, COMP *foff_phase_rect, int nin);

#endif //__FDMDV2_MAIN__
