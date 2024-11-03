# RaspberryPi를 이용한 HTTPS server 구현

## 프로젝트 주제

- 라즈베리파이를 활용하여 HTTPS 프로토콜을 지원하는 안전한 웹 서버를 구현하는 프로젝트이다.  OpenSSL을 이용한 SSL/TLS 인증서를 설정하여 HTTP 기반의 서버를 HTTPS로 전환하고 안전한 HTTPS 통신 환경을 구축한다.

## 프로젝트 목적

- 이 프로젝트는 라즈베리파이와 OpenSSL을 사용하여 HTTPS 웹 서버를 구축하는 방법을 익히는 것을 목표로 한다. 이를 통해 SSL/TLS를 이용한 보안 통신의 원리를 이해하고 서버의 안전한 통신을 구현하는 과정을 파악하고 라즈베리파이와 같은 임베디드 장치에서도 HTTPS와 같은 보안 기능을 활용할 수 있다는 것을 이해한다.

## 프로젝트 유형

- 개인 프로젝트

## 개발 인원 / 개발 기간

- 개발 인원 : 박경준
- 개발 기간 : 2024-10-06 ~ 2024-10-25

## 기술 스택

- **주요 언어**
    - C언어
- **개발 환경**
    - Linux
- **개발 도구**
    - OpenSSL
    - Raspberry Pi Model 4

## 핵심 기능

**SSL/TLS 인증서 생성 및 적용**

- openssl을 사용하여 자체 서명된 SSL 인증서와 비밀 키를 생성하고 서버에서 이를 이용해 HTTPS 통신 처리

**SSL 인증서 생성 방법**

![33](https://github.com/user-attachments/assets/92bf7922-8125-458f-837c-239b2d72c91f)

→ 자체 서명된 인증서를 사용하기 위해 new_cert.pem과 new_key.pem 파일을 직접 만든다.

**HTTPS 서버 구현**

- openssl 라이브러리를 이용하여 SSL/TLS 기반의 안전한 통신을 지원하는 웹 서버 구현
- 서버에서 자체 서명된 인증서를 생성하고 이를 통해 HTTPS 프로토콜 지원

![image](https://github.com/user-attachments/assets/fdee41d3-d44f-4339-914a-ed474714e6df)

SSL/TLS 라이브러리 초기화와 모든 암호화 알고리즘 추가

![res3](https://github.com/user-attachments/assets/5e2e2791-11b6-44fc-9703-15673396f94e)

https server 실행

**클라이언트 요청 처리**

- HTTP GET 요청을 처리하고 클라이언트(브라우저)에게 파일을 반환하는 기능 구현
- HTTPS로 요청을 받아 암호화된 데이터 처리
- 클라이언트와 서버 간 모든 데이터가 SSL/TLS를 통해 암호화되어 전송되므로 MITM(man in the middle attack)같은 공격과 데이터 도청을 방지

![image 1](https://github.com/user-attachments/assets/58fd11e8-bd08-4e2d-9164-0b74e6107de9)

![%EC%8A%A4%ED%81%AC%EB%A6%B0%EC%83%B7_2024-11-03_161750](https://github.com/user-attachments/assets/e502f45f-d087-4221-90cd-f914873298da)

→자체 서명된 인증서이기 때문에 브라우저에서 접속 시 보안 경고가 나타난다. 자체 서명 인증서를 사용하지 않고 공인 인증서를 도입한다면 해결 가능함