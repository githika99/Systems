### Unit tests

This directory contains tests and utilities for the
http server.

#### Basic Tests

* get_asmall.sh: sends a get request for a small file (the lyrics to
  Taylor Swift's Antihero).  Validates that the correct status code
  (200) and message body (the file contents) are returned by your
  server.

* put_new_asmall.sh: sends a put request for a small file that is new
  (i.e., there is no file at the URI specified in the put request).
  Validates that the correct status code (i.e., 201) and message body
  (Created\n) are returned in the response from your server.

* put_overwrite_asmall.sh: sends a put request for a small that is
  already contained in the server's directory (i.e., there is already
  a file at the URI specified in the put request).  Validates that the
  correct status code (200) and message body (OK\n) are returned by
  your server.

* iget_nofile.sh: sends a get request for a file that does not exist.
  Validates that the correct status code (404) and message body (Not
  Found\n) are returned by your server.

* invalid_version.sh: sends a request that has a malformed version
  (HTTP/1.10).  Validates that the correct status (400) code and message
  body (Bad Request\n) are returned by your server.

