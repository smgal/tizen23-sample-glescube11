Samsung의 내부 직무 변경 면접을 통과하여 bada 개발팀에 합류하면서 처음으로 만들었던 샘플

내가 맡게된 graphics 파트에서도 SDK에 공식 샘플이 들어가는 것으로 되어 있었는데 당시의 샘플은 orthogonal 공간에서 큐브가 도는 것이었음
이대로 샘플이 전세계로 나간다면 아주 부끄러울 것 같아서 빠르게 샘플을 새로 만들게 됨

OpenGL 1.1의 스펙을 거의 모두 테스트 가능하도록 만들었는데, 아래의 것이 하나의 화면에서 모두 확인 가능하도록 하는 것임

- clear color / vertex color
- frustum / fog
- translate / rotate
- fixed float
- face culling / depth test
- texture 565 / 4444
- EGL의 PBuffer / Pixmap

(그 뒤로 OpenGL 2.0 샘플도 더 좋게 바꿈)

최초 버전은 이렇게 나갔지만 나중에 PowerVR이 아닌 칩을 사용하게 되면서 PBuffer 등은 문제가 많아서 몇 년 뒤에 삭제...
나중에 Tizen이 bada를 흡수하면서 이 샘플도 같이 흡수가 되었고, SDK 샘플의 제작 책임은 인도 연구소 쪽으로 넘어감

Tizen SDK가 나가기 전에 Tizen용 이 샘플을 보게 되었는데, 원래의 취지와는 다른(잘 못 코딩된) 부분이 너무 많았음
그래서 주말에 Tizen용으로 새로 만들어서 담당자에게 보내 준 것이 바로 이것.
