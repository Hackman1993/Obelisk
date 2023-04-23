FROM alpine
RUN sed -ri -e 's!dl-cdn.alpinelinux.org!mirrors.aliyun.com!g' /etc/apk/repositories

