#include <libssh/libssh.h>
#include <stdlib.h>
#include <string>

const char pwd[] = "raspberry";
const char usr[] = "pi@10.4.51.118";
const char cmd[] = "ls";

int run_ssh_cmd(ssh_session session, const char command[]);

int main()
{
  //declaring session
  ssh_session my_ssh_session;
  int rc;
 
  // Open session and set options
  my_ssh_session = ssh_new();
  if (my_ssh_session == NULL)
    exit(-1);
  ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, usr);
 
  // Connect to server
  rc = ssh_connect(my_ssh_session);
  if (rc != SSH_OK)
  {
    fprintf(stderr, "Error connecting to localhost: %s\n",
            ssh_get_error(my_ssh_session));
    ssh_free(my_ssh_session);
    exit(-1);
  }
 
  // Authenticate ourselves
  rc = ssh_userauth_password(my_ssh_session, NULL, pwd);
  if (rc != SSH_AUTH_SUCCESS)
  {
    fprintf(stderr, "Error authenticating with password: %s\n",
            ssh_get_error(my_ssh_session));
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    exit(-1);
  }
 
  //running the command
  run_ssh_cmd(my_ssh_session, cmd);
 
  //disconnecting from ssh session
  ssh_disconnect(my_ssh_session);
  ssh_free(my_ssh_session);
}

int run_ssh_cmd(ssh_session session, const char command[]){
  ssh_channel channel;
  int rc;
 
  channel = ssh_channel_new(session);
  if (channel == NULL){ return SSH_ERROR;}
 
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK){
    ssh_channel_free(channel);
    return rc;
  }

  rc = ssh_channel_request_exec(channel, command);
  if (rc != SSH_OK){
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return rc;
  }

  char buffer[256];
  int nbytes;
  
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  while (nbytes > 0)
  {
    if (fwrite(buffer, 1, nbytes, stdout) != nbytes)
    {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      return SSH_ERROR;
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }
  
  if (nbytes < 0)
  {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return SSH_ERROR;
  }

  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
 
  return SSH_OK;
}
