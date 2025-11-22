#ifndef SEEK_AUDIO_AEC_H
#define SEEK_AUDIO_AEC_H 1



/**
* 这是一个通过AI模型推理和传统nlms(PBFDAF)算法相结合实现回音消除的双引擎架构aec，通过ai引擎赋能后，它不但具备强大的回音消除功能，还具备良好的啸叫抑制功能。
* 用户可以根据需要自行选择是否开启ai引擎。并且还可以自行决定输出AI引擎的多少性能。
* 如果不开启AI引擎，它就是一个传统算法实现但性能比webrtc aec3更好的传统aec。
* 用户可以在下面的仓库了解具体的调用方法：https://github.com/seekaudio/webrtc-for-seekaudio 
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SEEKAUDIO_E
# if defined(WIN32)
#   define SEEKAUDIO_E __declspec(dllexport)
# elif defined(__GNUC__)
#  define SEEKAUDIO_E __attribute__ ((visibility ("default")))
# else
#  define SEEKAUDIO_E
# endif
#else
#  define SEEKAUDIO_E
#endif

typedef void AECHandle;   //a handle for AEC


/**
 * 创建和返回aec对象
 */
SEEKAUDIO_E AECHandle* SeekAudioAEC_Create();



/*
 * 释放aec对象
 */
SEEKAUDIO_E void SeekAudioAEC_Free(AECHandle* AEC_inst);



/*
 * 该函数可以打印日志，必须要在SeekAudioAEC_Init之前调用
 *
 * 输入:
 *      - AEC_inst   : AEC 对象.
 *      - folderPath : 这是文件夹路径(不是文件路径)，比如：SeekAudioAEC_OpenLog(inst, "D:/test/")，调用了此函数后，会在该文件夹生成dump后缀的日志文件，
		  文件名根据当前的系统时间自动生成。该参数可以为NULL，然后日志文件会被保存在默认的程序安装目录中
 * 返回值      :  0 - 成功
 *               -1 - 失败
 */
SEEKAUDIO_E int SeekAudioAEC_OpenLog(AECHandle* AEC_inst,const char *folderPath);



/*
 * 该函数用来初始化aec对象，
 *
 * 输入:
 *      - AEC_inst       : 对象
 *      - sampleRate    : 采样率，目前仅支持16000采用率
 *
 * 返回值               :  0 - 成功
 *                        -1 - 失败
 */
SEEKAUDIO_E int SeekAudioAEC_Init(AECHandle* AEC_inst, int sampleRate);



/*
 * 该函数用来设置AI引擎输出多大的性能给啸叫抑制功能，由于AI输出啸叫抑制的强度过大可能会带来音质的改变，所以用户可以通过此函数在音质和啸叫抑制效果之间找到平衡点
 * 该函数应该在SeekAudioAEC_Init之后就调用，也可以运行时调用，但是必须要和SeekAudioAEC_Process处在同一个线程中。
 * 输入:
 *      - AEC_inst      :  对象
 *      - powerLevel    :  百分比的值，数值范围是0-100，默认值是0.比如，powerLevel==35表示AI引擎会输出35%的性能给啸叫抑制功能。如果powerLevel为0或者不调用此函数，表示没有任何啸叫抑制作用。
 *                         一旦powerLevel大于0，ai引擎会被激活，cpu占用会大幅提升
 * 返回值               void
 */
SEEKAUDIO_E void SeekAudioAEC_Set_AI_Engine_Power_For_Howl(AECHandle* AEC_inst, int powerLevel);


/*
 * 该函数用来设置AI引擎输出多大的性能给回音消除功能，由于AI输出回音压制的强度过大可能会带来音质的改变，所以用户可以通过此函数在音质和回音消除效果之间找到平衡点
 * 该函数应该在SeekAudioAEC_Init之后就调用，也可以运行时调用，但是必须要和SeekAudioAEC_Process处在同一个线程中。
 * 大多数情况下，不建议调用此函数，因为powerLevel为0时它是一个运算量不大但回音抑制效果比webrtc aec3更好的传统aec。只有出现回音时才建议调用此函数
 * 输入:
 *      - AEC_inst       : 对象
 *      - powerLevel    :  百分比的值，数值范围是0-100.默认值是0.比如，powerLevel==35表示AI引擎会输出35%的性能给回音消除功能。如果powerLevel为0或者不调用此函数，表示未启用ai引擎的aec功能,只有传统的aec回音消除功能
                           一旦powerLevel大于0，AI引擎会被激活，cpu占用会大幅提升
 *
 * 返回值               void
 */
SEEKAUDIO_E void SeekAudioAEC_Set_AI_Engine_Power_For_Echo(AECHandle* AEC_inst, int powerLevel);


/*
 * 该函数用来执行啸叫抑制和回音消除处理，输入的nearend数据大小必须是160个采样的音频数据大小(比如:160*sizeof(short))，输出也一样。
 *
 * 输入
 *      - AEC_inst       :AEC 对象
 *      - nearend       : 近端音频数据,数据大小是160个采样的音频数据大小160*sizeof(short)
 *      - nrOfSamples       : 输入的音频采样的大小，该值应该是160；
 *
 * 输出:
 *      - outframe      : 输出的音频数据，数据长度是160个采样的音频数据大小160*sizeof(short)

 * 返回值               :  0 - 成功
 *                        -1 - 失败
 */
SEEKAUDIO_E int SeekAudioAEC_Process(AECHandle* AEC_inst, const short* nearend, short* outframe, int nrOfSamples);



/*
 * 该函数用来缓存远端的音频数据，输入的farend数据大小必须是160个采样的音频数据大小(比如:160*sizeof(short))
 *
 * 输入
 *      - AEC_inst       : 对象
 *      - farend       : 远端音频数据,数据大小必须是160个采样的音频数据大小160*sizeof(short)
 *      - nrOfSamples       : 远端的音频采样的大小，该值应该是160；
 *
 * 输出: 无
 * 返回值               :  0 - 成功
 *                        -1 - 失败
 */
SEEKAUDIO_E int SeekAudioAEC_buffer_farend(AECHandle* AEC_inst, const short* farend, int nrOfSamples);


/*
 * 如果使用了agc进行放大音量处理，那么该函数可以通过agc补偿，进一步压制啸叫抑制(此函数可以不调用)。如果没有使用agc或者没有使用其它类似放大音量的处理，该函数应该被忽略。
 * 啸叫抑制的处理主要还是靠SeekAudioAEC_Process来实现，SeekAudioAEC_AGC_Compensate函数只是起到辅助作用。
 * Input
 *      - AEC_inst      : 对象
 *      - agc_in        : agc模块处理之前的音频数据
 *      - agc_out       : agc模块处理之后输出的音频数据
 *      - inframe       : SeekAudioAEC_Process函数处理之后输出的音频数据
 *
 * Output:
 *      - outframe      : 经过agc补偿后的音频输出数据.
 */
SEEKAUDIO_E void SeekAudioAEC_AGC_Compensate(AECHandle* AEC_inst, const short* agc_in, const short* agc_out, const short* inframe, short* outframe);




/*
 * 获取音频啸叫状态，该接口是保留接口，还没实现。
 *
 * Input
 *      - AEC_inst      : 对象.
 *
 * Return value         :  输出啸叫的概率值， 范围是 >=0 和 <=1 ;
 *
 */
SEEKAUDIO_E float SeekAudioAEC_GetHowlingStatus(AECHandle* AEC_inst);

#ifdef __cplusplus
}
#endif

#endif
