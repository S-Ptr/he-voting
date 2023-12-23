import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AdminCreatepollComponent } from './admin-createpoll/admin-createpoll.component';
import { VoterSubmitvoteComponent } from './voter-submitvote/voter-submitvote.component';
import { LoginComponent } from './login/login.component';
import { AdminLoginComponent } from './admin-login/admin-login.component';

const routes: Routes = [
  {path:"admin/create-poll",component:AdminCreatepollComponent},
  {path:'voter/submit-vote/:identifier',component: VoterSubmitvoteComponent},
  {path:'login',component: LoginComponent},
  {path:'admin-login',component: AdminLoginComponent},
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
