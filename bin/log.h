#ifndef _LOG_H_
#define _LOG_H_

extern int _war2edit_log_dom;

#ifdef ERR
# undef ERR
#endif

#define DBG(...) EINA_LOG_DOM_DBG(_war2edit_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_war2edit_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_war2edit_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_war2edit_log_dom, __VA_ARGS__)
#define CRI(...) EINA_LOG_DOM_CRIT(_war2edit_log_dom, __VA_ARGS__)

Eina_Bool log_init(void);
void log_shutdown(void);

#endif /* ! _LOG_H_ */

