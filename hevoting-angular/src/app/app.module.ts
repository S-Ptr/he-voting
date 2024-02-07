import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { VoterSubmitvoteComponent } from './voter-submitvote/voter-submitvote.component';
import { LoginComponent } from './login/login.component';
import { AdminCreatepollComponent } from './admin-createpoll/admin-createpoll.component';
import {ReactiveFormsModule} from '@angular/forms';

import { FormsModule } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';
import { UserPollComponent } from './user-poll/user-poll.component';
import { VoterMainComponent } from './voter-main/voter-main.component';
import { AdminLoginComponent } from './admin-login/admin-login.component';
import { RegisterComponent } from './register/register.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { AdminRegisterComponent } from './admin-register/admin-register.component';
import { LandingPageComponent } from './landing-page/landing-page.component';

import {MatIconModule} from '@angular/material/icon';
import {MatButtonModule} from '@angular/material/button';
import {MatToolbarModule} from '@angular/material/toolbar';
import {MatCheckboxModule} from '@angular/material/checkbox';
import {MatProgressSpinnerModule} from '@angular/material/progress-spinner';


@NgModule({
  declarations: [
    AppComponent,
    VoterSubmitvoteComponent,
    LoginComponent,
    AdminCreatepollComponent,
    UserPollComponent,
    VoterMainComponent,
    AdminLoginComponent,
    RegisterComponent,
    AdminRegisterComponent,
    LandingPageComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    FormsModule,
    ReactiveFormsModule,
    HttpClientModule,
    BrowserAnimationsModule,
    MatButtonModule,
    MatIconModule,
    MatToolbarModule,
    MatCheckboxModule,
    MatProgressSpinnerModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
