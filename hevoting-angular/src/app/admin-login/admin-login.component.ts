import { Component, OnInit } from '@angular/core';
import { UserService } from '../user.service';
import { Router } from '@angular/router';

@Component({
  selector: 'app-admin-login',
  templateUrl: './admin-login.component.html',
  styleUrls: ['./admin-login.component.css']
})
export class AdminLoginComponent {

  constructor(private router: Router, private userservice: UserService) { }

  ngOnInit(): void {
  }

  username: string = ""
  pass: string = ""
  msg: string = ""


  PrijaviSe() {

    let data = {
      username:this.username,
      pass:this.pass
    }

    this.userservice.adminLogin(data).subscribe((data:any) => {
      if (data) {
        let user = {name:data.name, type:"admin"}
        localStorage.setItem('user', JSON.stringify(user));
        this.router.navigate(['/voter-main']);
        this.router.navigate(["/admin/create-poll"]);
      }
      else {
        this.msg = 'Neispravno uneti podaci';
      }
    })


  }
}
