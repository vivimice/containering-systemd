# Copyright 2024 vivimice <vivimice@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

FROM vivimice/containering-systemd:debian-12-slim as contaiering-systemd

# Use fedora:40 as base image
FROM fedora:40

# Install systemd
# It's recommended to mask unnecessary units ship from the distro
RUN dnf -y install systemd && dnf clean all

# Copy journalagent from containering-systemd image and enable it
COPY --from=contaiering-systemd --chmod=0644 /usr/lib/libjournalagent_interceptor.so          /usr/lib/
COPY --from=contaiering-systemd --chmod=0755 /usr/bin/journalagent /usr/bin/systemd-bootstrap /usr/bin/
COPY --from=contaiering-systemd --chmod=0644 /etc/systemd/system/journalagent.service         /etc/systemd/system/
RUN systemctl enable journalagent

# journalagent expect systemd and journalctl can be found under /usr/bin
RUN ln -sf /lib/systemd/systemd /usr/bin/systemd


# Define stop signal for systemd
STOPSIGNAL SIGRTMIN+3

ENTRYPOINT [ "/usr/bin/systemd-bootstrap" ]