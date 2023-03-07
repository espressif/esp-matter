#ifndef SL_MVP_POWER_H
#define SL_MVP_POWER_H

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
/***************************************************************************//**
 * @addtogroup mvp MVP API
 * @{
 ******************************************************************************/

/**
 * @brief
 *   Initialize mvp power handling.
 *
 * @details
 *   This function is called by sli_mvp_init(). If power manger is used then
 *   this function will also register an internal callback to be triggered
 *   whenever the application enters/exits EM2 or EM3. This callback will call
 *   sli_mvp_power_down() before EM2/EM3 and sli_mvp_power_up() after EM2/EM3
 *   wakeup.
 */
void sli_mvp_power_init(void);

/**
 * @brief
 *   De-initialize mvp power handling.
 *
 * @details
 *   This function is called by sli_mvp_init().
 */
void sli_mvp_power_deinit(void);

/**
 * @brief
 *   Power up the MVP hardware.
 *
 * @details
 *   This function will restore the MVP clocks and register state, typically
 *   after an EM2 or EM3 wakeup. This function is also called by sli_mvp_init()
 *   in order to setup mvp clocks.
 */
void sli_mvp_power_up(void);

/**
 * @brief
 *   Power down the MVP hardware.
 *
 * @details
 *   This function will save MVP register state and turn off all the MVP clocks.
 *   This should be done before entering EM2 or EM3.
 */
void sli_mvp_power_down(void);

/**
 * @brief
 *   Internal function called before any program is loaded to the MVP hardware.
 *
 * @details
 *   This function is used internally to prepare the MVP for loading a MVP program.
 *   Preparing for execution involves waiting for the MVP to finish execution
 *   of the currently running program. The SL_MVP_POWER_MODE configuration will
 *   control how this function behaves.
 */
void sli_mvp_power_program_prepare(void);

/**
 * @brief
 *   Internal function called when waiting for an MVP program to finish.
 *
 * @details
 *   This function is used internally to wait for the MVP to finish execution
 *   of it's currently loaded program. The SL_MVP_POWER_MODE configuration will
 *   control how this waiting is done.
 */
void sli_mvp_power_program_wait(void);

/**
 * @brief
 *   Internal function called when a MVP program finish execution.
 *
 * @details
 *   This function is used internally to signal that an MVP program is finished
 *   executing. The SL_MVP_POWER_MODE configuration will control how this
 *   function behaves.
 */
void sli_mvp_power_program_complete(void);

/** @} (end addtogroup mvp) */
/// @endcond

#ifdef __cplusplus
}
#endif

#endif
