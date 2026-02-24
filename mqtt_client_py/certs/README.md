# Certificates Directory

This directory is used to store TLS certificates required for secure MQTT communication.
## Required Files

Depending on your MQTT broker configuration, you may need:

### 1. CA Certificate (required)
Used to verify the MQTT broker identity.

**File name:** ca.crt  

Only this file name should be used in this project.

The file must be in **PEM format** and must begin with:
-----BEGIN CERTIFICATE-----

and end with 
-----END CERTIFICATE-----