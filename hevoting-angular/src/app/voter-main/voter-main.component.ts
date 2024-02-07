import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { VotingService } from '../voting.service';

@Component({
  selector: 'app-voter-main',
  templateUrl: './voter-main.component.html',
  styleUrls: ['./voter-main.component.css']
})
export class VoterMainComponent {

  polls:any;

  constructor(private router:Router, private votingservice:VotingService){}

  ngOnInit(): void {
    this.votingservice.getAllPolls().subscribe((resp:any)=>{
      this.polls = resp.polls;
    })
  }
}
