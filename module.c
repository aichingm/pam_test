#include <security/pam_appl.h>
#include <security/pam_ext.h>
#include <security/pam_modules.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define init_log()                 \
  setlogmask(LOG_UPTO(LOG_DEBUG)); \
  openlog(PROG_NAME, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

#define RETURN(code) \
  closelog();        \
  return code;

#define PROG_NAME "pam_test"

PAM_EXTERN int pam_sm_setcred(pam_handle_t* pamh,
                              int flags,
                              int argc,
                              const char** argv) {
  init_log();
  syslog(LOG_ERR, "this module does not support setcred via pam_sm_setcred");
  return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh,
                                   int flags,
                                   int argc,
                                   const char** argv) {
  int retval;
  const char* user;
  const char* password;

  init_log();

  if (argc != 2) {
    syslog(
        LOG_ERR,
        "the module expects a user name and a password from the service file");
    RETURN(PAM_AUTH_ERR);
  }

  syslog(LOG_DEBUG, "argv[%d]: %s, %s", argc, argv[0], argv[1]);

  retval = pam_get_user(pamh, &user, "Username: ");
  if (retval != PAM_SUCCESS) {
    RETURN(retval);
  }
  if (strcmp(user, argv[0]) != 0) {
    syslog(LOG_NOTICE, "unknown user");
    RETURN(PAM_USER_UNKNOWN);
  }

  retval = pam_get_authtok(pamh, PAM_AUTHTOK, &password, "Password");
  if (retval != PAM_SUCCESS) {
    syslog(LOG_NOTICE, "password mismatch");
    RETURN(retval);
  }

  retval = strcmp(password, argv[1]) == 0;
  user = password = NULL;
  if (!retval) {
    syslog(LOG_NOTICE, "password mismatch");
    RETURN(PAM_AUTH_ERR);
  }

  RETURN(PAM_SUCCESS);
}
