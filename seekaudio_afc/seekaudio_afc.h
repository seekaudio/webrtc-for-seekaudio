#ifndef SEEK_AUDIO_AFC_H
#define SEEK_AUDIO_AFC_H 1

/**
* 这是一个通过AI模型推理实现的防啸叫抑制算法库，通过ai引擎赋能后，将具备良好的啸叫抑制功能。用户可以根据需要自行选择是否开启ai引擎，并且还可以自行决定输出AI引擎的多少性能
* 如果不开启AI引擎，它就是一个传统算法实现的基本降噪库，目前它的降噪性能几乎和webrtc ns模块差不多，但随着后续进一步的优化和升级，它将会远超webrtc ns模块
* 用户可以在下面的仓库了解具体的调用方法：https://github.com/seekaudio/webrtc-for-seekaudio
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEEKAUDIO_EXPORT
# if defined(WIN32)
#   define SEEKAUDIO_EXPORT __declspec(dllexport)
# elif defined(__GNUC__)
#  define SEEKAUDIO_EXPORT __attribute__ ((visibility ("default")))
# else
#  define SEEKAUDIO_EXPORT
# endif
#else
#  define SEEKAUDIO_EXPORT
#endif

typedef void AFCHandle; 

/**
 * 创建和返回afc对象
 */
SEEKAUDIO_EXPORT AFCHandle* SeekAudioAFC_Create();

/*
 * 释放afc对象
 */
SEEKAUDIO_EXPORT void SeekAudioAFC_Free(AFCHandle* AFC_inst);


/*
 * 函数可以打印日志，必须要在SeekAudioAFC_Init之前调用
 *
 * 输入:
 *      - AFC_inst   : AFC 对象.
 *      - folderPath : 这是文件夹路径(不是文件路径)，比如：SeekAudioAFC_OpenLog(inst, "D:/test/")，调用了此函数后，会在该文件夹生成dump后缀的日志文件，
          文件名根据当前的系统时间自动生成。该参数可以为NULL，然后日志文件会被保存在默认的程序安装目录中
 * 返回值      :  0 - 成功
 *               -1 - 失败
 */
SEEKAUDIO_EXPORT int SeekAudioAFC_OpenLog(AFCHandle* AFC_inst,const char *folderPath);

/*
 * 该函数用来初始化afc对象，
 *
 * 输入:
 *      - AFC_inst       : 对象
 *      - sampleRate    : 采样率，目前仅支持16000采样率
 *
 * 返回值               :  0 - 成功
 *                        -1 - 失败
 */
SEEKAUDIO_EXPORT int SeekAudioAFC_Init(AFCHandle* AFC_inst, int sampleRate);


/*
 * 该函数用来设置AI引擎输出多大的性能，由于AI输出啸叫抑制的强度过大可能会带来音质的改变，所以用户可以通过此函数在音质和啸叫抑制效果之间找到平衡点
 * 该函数应该在SeekAudioAFC_Init之后就调用，也可以运行时调用，但是必须要和SeekAudioAFC_Process处在同一个线程中。
 * 输入:
 *      - AFC_inst       : 对象
 *      - powerLevel    :  百分比的值，默认值是0，数值范围是0-100.比如，powerLevel==35表示AI引擎会输出35%的啸叫抑制性能。如果powerLevel为0或者不调用此函数，不会有任何啸叫抑制作用。
 *                         一旦powerLevel大于0，ai引擎会被激活，cpu占用会大幅提升
 *
 * 返回值               void
 */
SEEKAUDIO_EXPORT void SeekAudioAFC_Set_AI_Engine_Power(AFCHandle* AFC_inst, int powerLevel);

/*
 * 该函数用来执行啸叫抑制处理，如果没有啸叫会输出原始音频数据，如果有啸叫出现，会输出啸叫抑制后的音频。输入的inframe数据大小必须是160个采样的音频数据大小(比如:160*sizeof(float))，输出也一样。
 *
 * 输入
 *      - AFC_inst       :AFC 对象
 *      - inframe       : 输入的音频数据,数据长度是160个采样的音频数据大小，比如160*sizeof(short)
 *
 * 输出:
 *      - outframe      : 输出的音频数据
 */
SEEKAUDIO_EXPORT void SeekAudioAFC_Process(AFCHandle* AFC_inst,const short* inframe, short* outframe);
/*
 * 如果使用了agc进行放大音量处理，那么该函数可以通过agc补偿，进一步压制啸叫抑制(此函数可以不调用)。如果没有使用agc或者没有其它类似放大音量的处理，该函数应该忽略。
 * 啸叫抑制的处理主要还是靠SeekAudioAFC_Process来实现，SeekAudioAFC_AGC_Compensate函数只是起到辅助作用。
 * Input
 *      - AFC_inst      :AFC 对象
 *      - agc_in        : agc模块处理之前的音频数据
 *      - agc_out       : agc模块处理之后输出的音频数据
 *      - inframe       : SeekAudioAFC_Process函数处理之后输出的音频数据
 *
 * Output:
 *      - outframe      : 经过agc补偿后的音频输出数据.
 */
SEEKAUDIO_EXPORT void SeekAudioAFC_AGC_Compensate(AFCHandle* AFC_inst, const short* agc_in, const short* agc_out, const short* inframe, short* outframe);


/*
 * 获取音频啸叫状态，该接口是保留接口，还没实现。
 *
 * Input
 *      - AFC_inst      :AFC 对象.
 *
 * Return value         :  输出啸叫的概率值， 范围是 >=0 和 <=1 ;
 *                        
 */
SEEKAUDIO_EXPORT float SeekAudioAFC_GetHowlingStatus(AFCHandle* AFC_inst);

#ifdef __cplusplus
}
#endif

#endif
