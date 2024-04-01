import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { VoterSubmitvoteComponent } from './voter-submitvote/voter-submitvote.component';
import { LoginComponent } from './login/login.component';
import { AdminCreatepollComponent } from './admin-createpoll/admin-createpoll.component';
import {ReactiveFormsModule} from '@angular/forms';

import { FormsModule, Validators, FormControl } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';
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
import {MatFormFieldModule} from '@angular/material/form-field';
import {MatInputModule} from '@angular/material/input';
import {MatCardModule} from '@angular/material/card';
import {MatSnackBarModule} from '@angular/material/snack-bar';

import {ClipboardModule} from '@angular/cdk/clipboard';
import { LogoutComponent } from './logout/logout.component';

@NgModule({
  declarations: [
    AppComponent,
    VoterSubmitvoteComponent,
    LoginComponent,
    AdminCreatepollComponent,
    VoterMainComponent,
    AdminLoginComponent,
    RegisterComponent,
    AdminRegisterComponent,
    LandingPageComponent,
    LogoutComponent
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
    MatProgressSpinnerModule,
    MatInputModule,
    MatFormFieldModule,
    MatCardModule,
    MatSnackBarModule,
    ClipboardModule,
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
