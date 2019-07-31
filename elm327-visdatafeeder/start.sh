#!/bin/bash

sleep 0.5s

echo "Starting ELM 327 app"


SERIAL='/dev/ttyAMA0'
TOKEN='eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJFeGFtcGxlIEpXVCIsImlzcyI6IkVjbGlwc2Uga3Vrc2EiLCJhZG1pbiI6dHJ1ZSwiaWF0IjoxNTE2MjM5MDIyLCJleHAiOjE2MDkzNzI4MDAsInczYy12c3MiOnsiU2lnbmFsLk9CRC4qIjoicncifX0.o5HF6H4u3boInC2d0NfR1k-dxiPOueblq_gvozpL24r0_z4l6gUYgLWBbI9fHsyjRu3ZXMG2enBzWwsCKi4KG1e5rBAfBh61oAncKdjvZExR0wOx6TF6DMcLCCIiRztV4q97DQgFU-k7FV4BsGtMwKGe1BhAT7_drqo7Vrxl2u-sDZuFeF3flyn-a_vo_NjfMEl7UcSr4aIp0o6uuGnBGYZJk0wYiV-B6s93b0eGWSQAPcvNhpCikbpBmzR1-dtLinQLt--0aQEWzVtjZBrWWAW-5FlhQ42qBYqxbhXGquag1ylhqD3J1nJgIIFZv3b1DBoQQSv7lXY-m2flDEvjaUVXevmDYhwoJvsabVFBdtLf3aURvttbO8XWhCDrA8-ApdnjZPvmoZv8BCwnWSrCECL4V7-NkKuarG_YlrZKswjzO5OFFQYEHaOMo-rSWWSRAIV7fBQP_ryYy6ZCaFuS3K6ZrvotywIqUhSleUxLAit_cwid5XxEwoYbIdykXl-kO8hrr9naJYE6ISvPWPx7N2A71CQJ-djsOlyOTZ6Emuc7lBnGgRybFwjE7YSz8ye-SROEnUq5fhKnIWwce66uGV93NjqWbFW1usH9J5DvG-UcJj41IDXeDUfzygIqHoRGgRNzIyww82JC5JBEzx1svTLigXfYVq2Z1m-sg-bSkQM'


./elm327-visdatafeeder $SERIAL $TOKEN 50000 3000000

exit_code=$?

exit $exit_code
