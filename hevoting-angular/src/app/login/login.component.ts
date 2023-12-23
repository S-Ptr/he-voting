import { Component, OnInit } from '@angular/core';
import { UserService } from '../user.service';
import { Router } from '@angular/router';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {

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

    this.userservice.login(data).subscribe((data:any) => {
      if (data) {
        window.sessionStorage.setItem("user", data.username)
      }
      else {
        this.msg = 'Neispravno uneti podaci';
      }
    })


  }

}
