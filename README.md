# RaspberryPi를 이용한 HTTPS server 구현

## 프로젝트 소개
- 라즈베리파이를 활용하여 HTTPS 프로토콜을 지원하는 안전한 웹 서버를 구현했습니다. 이를 위해 OpenSSL을 사용하여 SSL/TLS 인증서를 생성 및 설정하고 기존 HTTP 기반 서버를 HTTPS로 전환하여 보안 통신 환경을 구축했습니다.

**설계 계획**
1. **HTTP-Server 구현**
    - 기본적인 HTTP 서버를 설계하고 구현하여 클라이언트 요청 처리 후 파일 데이터 제공 기능 개발
    - HTTP 프로토콜을 사용하여 서버-클라이언트간 통신 테스트

2. **HTTPS-Server 확장**
    - HTTP 서버를 기반으로 SSL/TLS 암호화를 적용하여 HTTPS 서버로 확장
    - OpenSSL 라이브러리를 활용하여 TLS 핸드셰이크와 데이터 암호화 구현
    - 자체 서명된 인증서를 생성하여 HTTPS 활성화

3. **Raspberry Pi Porting**
    - 개발한 HTTPS 서버를 라즈베리파이에 배포하여 임베디드 환경에서 실행 가능하도록 설계
    - 라즈베리파이에서 테스트 및 디버깅하여 네트워크 통신과 HTTPS 동작 확인

## 기술 스택

- **주요 언어**
    - C
- **개발 환경**
    - Linux
- **개발 도구**
    - OpenSSL
    - Raspberry Pi Model 4

## 주요 기능

**SSL/TLS 인증서 생성 및 적용**

- openssl을 사용하여 자체 서명된 SSL 인증서와 비밀 키를 생성하고 서버에서 이를 이용해 HTTPS 통신 처리



![33](https://github.com/user-attachments/assets/92bf7922-8125-458f-837c-239b2d72c91f)

SSL 인증서 생성 → 자체 서명된 인증서를 사용하기 위해 new_cert.pem과 new_key.pem 파일을 직접 만들었습니다.

<br><br>

**HTTPS 서버 구현**
- 서버에서 자체 서명된 인증서를 생성하고 이를 통해 HTTPS 프로토콜 지원
- openssl 라이브러리를 이용하여 SSL/TLS 기반의 안전한 통신을 지원하는 웹 서버 구현

HTTPS Server 실행

![res3](https://github.com/user-attachments/assets/5e2e2791-11b6-44fc-9703-15673396f94e)



![image](https://github.com/user-attachments/assets/654070a7-1351-4b67-85a4-97eea33d233b)

->현재 자체 서명 인증서를 사용했기 때문에 인증서 발급기관이 공인되지 않았습니다. 따라서 브라우저가 신뢰할 수 없다고 표시되었지만 SSL/TLS를 통한 암호화는 활성화 되어있기 때문에 HTTPS는 적용되어있습니다.

![스크린샷 2024-12-07 183053](https://github.com/user-attachments/assets/f9decb06-6e9b-45e2-be29-16f8be307f7e)

**WireShark를 이용한 HTTPS 패킷 분석**
- 서버-클라이언트간 TSL 핸드셰이크(client Hello -> Server Hello -> Change Cipher Spec -> Application Data) 확인 -> HTTPS 연결 설정 확인

<br><br>

**클라이언트 요청 처리**

- HTTP GET 요청을 처리하고 클라이언트(브라우저)에게 파일 데이터를 반환하는 기능 구현
- HTTPS로 요청을 받아 암호화된 데이터 처리
- 클라이언트와 서버 간 모든 데이터가 SSL/TLS를 통해 암호화되어 전송되므로 MITM(man in the middle attack)같은 공격과 데이터 도청을 방지

![스크린샷 2024-12-07 174736](https://github.com/user-attachments/assets/a09ec476-f302-4863-a99f-5a72dbe81b78)


![스크린샷 2024-12-07 174538](https://github.com/user-attachments/assets/d4b8eec0-fe9a-476d-b002-db7ab272c278)


### 향후 보완
- 현재는 자체 서명된 인증서를 사용하여 HTTPS를 구현했지만 공인 인증기관을 통한 공인인증서를 발급받아 적용해야 브라우저의 보안 경고를 제거할 수 있기 때문에 향후 Port forwarding을 깊게 공부하여 도메인 설정과 공인인증서 발급을 받아야겠다 생각했습니다.