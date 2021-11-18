#include <libssh/libssh.h>
#include <stdlib.h>
#include <string>

//const char pwd[] = parameters::user;
const char pwd[] = "raspberry";
const char usr[] = "pi@10.4.51.118";
const char cmd[] = "python hello.py";
const char shutdown_cmd[] = "sudo shutdown now";

int run_ssh_cmd(ssh_session& session, const char command[]);

int open_ssh(ssh_session& session, const char user[], const char password[]);

int close_ssh(ssh_session& session);

int main()
{
  ssh_session my_ssh_session;
  int rc;

  //opening ssh connection
  open_ssh(my_ssh_session, usr, pwd);
  //running the command
  run_ssh_cmd(my_ssh_session, cmd);
  
  //shutdown the pi
  //run_ssh_cmd(my_ssh_session, shutdown_cmd);

  //closing ssh_connection
  close_ssh(my_ssh_session);
}

int run_ssh_cmd(ssh_session& session, const char command[]){
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

int open_ssh(ssh_session& session, const char user[], const char password[]){
  int rc;
 
  // Open session and set options
  session = ssh_new();
  if (session == NULL)
    exit(-1);
  ssh_options_set(session, SSH_OPTIONS_HOST, user);
 
  // Connect to server
  rc = ssh_connect(session);
  if (rc != SSH_OK)
  {
    fprintf(stderr, "Error connecting to localhost: %s\n",
            ssh_get_error(session));
    ssh_free(session);
    exit(-1);
  }
 
  // Authenticate ourselves
  rc = ssh_userauth_password(session, NULL, password);
  if (rc != SSH_AUTH_SUCCESS)
  {
    fprintf(stderr, "Error authenticating with password: %s\n",
            ssh_get_error(session));
    ssh_disconnect(session);
    ssh_free(session);
    exit(-1);
  }
 return 0;
}


int close_ssh(ssh_session& session){
  ssh_disconnect(session);
  ssh_free(session);
  return 0;
}
