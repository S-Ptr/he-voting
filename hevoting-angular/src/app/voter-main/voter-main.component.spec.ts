import { ComponentFixture, TestBed } from '@angular/core/testing';

import { VoterMainComponent } from './voter-main.component';

describe('VoterMainComponent', () => {
  let component: VoterMainComponent;
  let fixture: ComponentFixture<VoterMainComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ VoterMainComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(VoterMainComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
