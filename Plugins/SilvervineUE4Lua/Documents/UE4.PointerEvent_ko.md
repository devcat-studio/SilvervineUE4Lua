# UE4 PointerEvent Library

UE4의 PointerEvent 관련된 기능을 지원하는 라이브러리입니다.

## UE4.PointerEvent.GetScreenSpacePosition(PointEvent)

화면 공간에서 커서의 위치를 반환합니다.

## UE4.PointerEvent.GetLastScreenSpacePosition(PointEvent)

입력 이벤트를 마지막으로 처리 할 때 화면 공간에서 커서의 위치를 반환합니다.

## UE4.PointerEvent.GetCursorDelta(PointEvent)

마지막 이벤트가 처리 된 이후 마우스가 이동한 거리를 반환합니다.

## UE4.PointerEvent.IsLeftMouseButtonDown(PointEvent)

현재 눌려진 마우스 버튼이 왼쪽 버튼인지 확인합니다. 인자 PointEvent는 FPointerEvent 구조체입니다.

## UE4.PointerEvent.IsMouseRightButtonDown(PointEvent)

현재 눌려진 마우스 버튼이 오른쪽 버튼인지 확인합니다. 인자 PointEvent는 FPointerEvent 구조체입니다.

## UE4.PointerEvent.GetEffectingButton(PointEvent)

이 이벤트를 일으킨 마우스 버튼 정보를 반환합니다. (EB_None 일 가능성이 있음) 반환 타입은 FKey입니다.

## UE4.PointerEvent.GetWheelDelta(PointEvent)

마지막 마우스 이벤트 이후 마우스 휠이 돌았는지에 대한 값을 반환합니다.

## UE4.PointerEvent.GetUserIndex(PointEvent)

이벤트의 원인이 된 사용자의 인덱스를 반환합니다.

## UE4.PointerEvent.GetPointerIndex(PointEvent)

포인터의 고유 식별자를 반환합니다 (예 : finger index)

## UE4.PointerEvent.GetTouchpadIndex(PointEvent)

이벤트를 생성 한 터치 패드의 인덱스를 반환합니다 (사용자 당 여러 개의 터치 패드가 있는 플랫폼의 경우)

## UE4.PointerEvent.IsTouchEvent(PointEvent)

이 이벤트가 (마우스가 아닌) 터치로 인한 결과인지 확인합니다.

## UE4.PointerEvent.GetGestureType(PointEvent)

터치 동작 유형을 반환합니다.

## UE4.PointerEvent.GetGestureDelta(PointEvent)

동일한 유형의 마지막 동작 이벤트 이후의 동작 값의 변경을 반환합니다.

------------------------------------
[라이브러리](Library_ko.md)로 돌아가기