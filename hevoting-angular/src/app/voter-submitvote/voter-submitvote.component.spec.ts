import { ComponentFixture, TestBed } from '@angular/core/testing';

import { VoterSubmitvoteComponent } from './voter-submitvote.component';

describe('VoterSubmitvoteComponent', () => {
  let component: VoterSubmitvoteComponent;
  let fixture: ComponentFixture<VoterSubmitvoteComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ VoterSubmitvoteComponent ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(VoterSubmitvoteComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
