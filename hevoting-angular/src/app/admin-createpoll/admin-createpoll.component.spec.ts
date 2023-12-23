import { ComponentFixture, TestBed } from '@angular/core/testing';

import { AdminCreatepollComponent } from './admin-createpoll.component';

describe('AdminCreatepollComponent', () => {
  let component: AdminCreatepollComponent;
  let fixture: ComponentFixture<AdminCreatepollComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ AdminCreatepollComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(AdminCreatepollComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
